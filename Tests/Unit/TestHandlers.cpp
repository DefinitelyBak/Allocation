#include <gtest/gtest.h>

#include "CommonFunctions.h"
#include "Services/UoW/FakeUnitOfWork.h"
#include "Services/MessageBus/MessageBus.h"
#include "Services/Exceptions/InvalidSku.h"
#include "Domain/Events/OutOfStock.h"
#include "Domain/Events/BatchCreated.h"
#include "Domain/Events/AllocationRequired.h"


namespace Allocation::Tests
{
    TEST(Handlers, test_for_new_product)
    {
        Services::UoW::FakeUnitOfWork uow;
        Services::MessageBus::Instance().Handle(uow, std::make_shared<Domain::Events::BatchCreated>("b1", "CRUNCHY-ARMCHAIR", 100));

        EXPECT_TRUE(uow.GetProductRepository().Get("CRUNCHY-ARMCHAIR"));
        EXPECT_TRUE(uow.IsCommited());
    }

    TEST(Handlers, test_for_existing_product)
    {
        Services::UoW::FakeUnitOfWork uow;
        auto& messbus = Services::MessageBus::Instance();
        messbus.Handle(uow, std::make_shared<Domain::Events::BatchCreated>("b1", "GARISH-RUG", 100));
        messbus.Handle(uow, std::make_shared<Domain::Events::BatchCreated>("b2", "GARISH-RUG", 99));

        auto product = uow.GetProductRepository().Get("GARISH-RUG");
        auto batches = product->GetBatches();
        auto references = batches | 
                        std::views::transform([](const auto& b) { return b.GetReference(); });

        EXPECT_NE(std::ranges::find(references, "b2"), references.end());
    }

    TEST(Handlers, test_returns_allocation)
    {
        Services::UoW::FakeUnitOfWork uow;
        auto& messbus = Services::MessageBus::Instance();
        messbus.Handle(uow, std::make_shared<Domain::Events::BatchCreated>("b1", "COMPLICATED-LAMP", 100));

        auto result = messbus.Handle(uow, std::make_shared<Domain::Events::AllocationRequired>("o1", "COMPLICATED-LAMP", 10));

        EXPECT_EQ(result.front(), "b1");
    }

    TEST(Handlers, test_errors_for_invalid_sku)
    {
        Services::UoW::FakeUnitOfWork uow;
        Services::MessageBus::Instance().Handle(
            uow, 
            std::make_shared<Domain::Events::BatchCreated>("b1", "AREALSKU", 100)
        );

        EXPECT_TRUE(ThrowsWithMessage<Services::Exceptions::InvalidSku>(
            [&]() {
                Services::MessageBus::Instance().Handle(
                    uow,
                    std::make_shared<Domain::Events::AllocationRequired>("o1", "NONEXISTENTSKU", 10)
                );
            },
            "Invalid sku NONEXISTENTSKU"
        ));
    }

    TEST(Handlers, test_commits)
    {
        Services::UoW::FakeUnitOfWork uow;
        auto& messbus = Services::MessageBus::Instance();
        messbus.Handle(uow, std::make_shared<Domain::Events::BatchCreated>("b1", "OMINOUS-MIRROR", 100));
        messbus.Handle(uow, std::make_shared<Domain::Events::AllocationRequired>("o1", "OMINOUS-MIRROR", 10));

        EXPECT_TRUE(uow.IsCommited());
    }

    TEST(Handlers, test_sends_email_on_out_of_stock_erro)
    {
        auto handler = [](Domain::IUnitOfWork& uow, std::shared_ptr<Domain::Events::OutOfStock> event) -> std::optional<std::string>
        {
            EXPECT_EQ(event->SKU, "POPULAR-CURTAINS");

            return std::nullopt;
        };

        auto& messbus = Services::MessageBus::Instance();

        messbus.Subscribe<Domain::Events::OutOfStock>(handler);

        Services::UoW::FakeUnitOfWork uow;
        messbus.Handle(uow, std::make_shared<Domain::Events::BatchCreated>("b1", "POPULAR-CURTAINS", 9));
        messbus.Handle(uow, std::make_shared<Domain::Events::AllocationRequired>("o1", "POPULAR-CURTAINS", 10));
    }

    /*
    TEST(Handlers, test_changes_available_quantity)
    {
        uow = FakeUnitOfWork()
        messagebus.handle(
            events.BatchCreated("batch1", "ADORABLE-SETTEE", 100, None), uow
        )
        [batch] = uow.products.get(sku="ADORABLE-SETTEE").batches
        assert batch.available_quantity == 100

        messagebus.handle(events.BatchQuantityChanged("batch1", 50), uow)

        assert batch.available_quantity == 50
    }

    def test_reallocates_if_necessary(self):
        uow = FakeUnitOfWork()
        event_history = [
            events.BatchCreated("batch1", "INDIFFERENT-TABLE", 50, None),
            events.BatchCreated("batch2", "INDIFFERENT-TABLE", 50, date.today()),
            events.AllocationRequired("order1", "INDIFFERENT-TABLE", 20),
            events.AllocationRequired("order2", "INDIFFERENT-TABLE", 20),
        ]
        for e in event_history:
            messagebus.handle(e, uow)
        [batch1, batch2] = uow.products.get(sku="INDIFFERENT-TABLE").batches
        assert batch1.available_quantity == 10
        assert batch2.available_quantity == 50

        messagebus.handle(events.BatchQuantityChanged("batch1", 25), uow)

        # order1 or order2 will be deallocated, so we'll have 25 - 20
        assert batch1.available_quantity == 5
        # and 20 will be reallocated to the next batch
        assert batch2.available_quantity == 30
    */
}