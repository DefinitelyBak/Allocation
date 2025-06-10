#include <gtest/gtest.h>

#include "Precompile.h"
#include "Common.h"
#include "CommonFunctions.h"
#include "UoWFixture.h"
#include "Services/UoW/SqlUnitOfWork.h"


namespace Allocation::Tests
{

    TEST_F(UoWFixture, test_uow_can_retrieve_a_batch_and_allocate_to_it)
    {
        InsertBatch(*session, "batch1", "HIPSTER-WORKBENCH", 100);
        
        Services::UoW::SqlUnitOfWork uow;
        auto product = uow.GetProductRepository().Get("HIPSTER-WORKBENCH");
        Domain::OrderLine line("o1", "HIPSTER-WORKBENCH", 10);
        product->Allocate(line);
        uow.GetProductRepository().Add(product);
        uow.Commit();

        auto batchRef = GetAllocatedBatchRef(*session, "o1", "HIPSTER-WORKBENCH");
        EXPECT_EQ(batchRef, "batch1");
    }

    TEST_F(UoWFixture, test_rolls_back_uncommitted_work_by_default)
    {
        {
            Services::UoW::SqlUnitOfWork uow;
            InsertBatch(uow.GetSession(), "batch1", "MEDIUM-PLINTH", 100);
        }

        Poco::Data::Statement select(*session);
        int count = 0;
        select << "SELECT COUNT(*) FROM batches",
            Poco::Data::Keywords::into(count),
            Poco::Data::Keywords::now;

        EXPECT_EQ(count, 0);
    }

    TEST_F(UoWFixture, test_rolls_back_on_error)
    {
        try
        {
            Services::UoW::SqlUnitOfWork uow;
            InsertBatch(uow.GetSession(), "batch1", "LARGE-FORK", 100);

            throw std::exception();
        }
        catch (const std::exception&)
        {}

        Poco::Data::Statement select(*session);
        int count = 0;
        select << "SELECT COUNT(*) FROM batches",
            Poco::Data::Keywords::into(count),
            Poco::Data::Keywords::now;

        EXPECT_EQ(count, 0);
    }

    void tryToAllocate(std::string orderid, std::string sku,
                    std::mutex& mutex, std::vector<std::string>& exceptions)
    {
        try
        {
            Domain::OrderLine line(orderid, sku, 10);
            Services::UoW::SqlUnitOfWork uow;
            auto product = uow.GetProductRepository().Get(sku);
            product->Allocate(line);
            uow.GetProductRepository().Add(product);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            uow.Commit();
        }
        catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(mutex);
            exceptions.push_back(e.what());
        }
    }

    TEST_F(UoWFixture, test_concurrent_updates_to_version_are_not_allowed)
    {
        std::string SKU = RandomSku();
        std::string batch = RandomBatchRef();
        InsertBatch(*session, batch, SKU, 100, 1);

        std::string order1 = RandomOrderId("1");
        std::string order2 = RandomOrderId("2");

        std::mutex exceptions_mutex;
        std::vector<std::string> exceptions;

        std::thread t1(tryToAllocate, order1, SKU, std::ref(exceptions_mutex), std::ref(exceptions));
        std::thread t2(tryToAllocate, order2, SKU, std::ref(exceptions_mutex), std::ref(exceptions));

        t1.join();
        t2.join();

        int version = 0;
        *session << "SELECT version_number FROM products WHERE sku = ?",
            Poco::Data::Keywords::use(SKU),
            Poco::Data::Keywords::into(version),
            Poco::Data::Keywords::now;
        EXPECT_EQ(version, 2);

        EXPECT_EQ(exceptions.size(), 1);
        EXPECT_NE(exceptions[0].find("could not serialize access"), std::string::npos);

        int order_count = 0;
        *session << R"(
            SELECT COUNT(DISTINCT orderid)
            FROM allocations
            JOIN order_lines ON allocations.orderline_id = order_lines.id
            JOIN batches ON allocations.batch_id = batches.id
            WHERE order_lines.sku = ?
        )",
        Poco::Data::Keywords::use(SKU),
        Poco::Data::Keywords::into(order_count),
        Poco::Data::Keywords::now;
        EXPECT_EQ(order_count, 1);
    }
}