#include <gtest/gtest.h>

#include "Forwards.h"
#include "CommonFunctions.h"
#include "Services.h"
#include "InvalidSku.h"
#include "Adapters/Repository/Include/FakeRepository.h"
#include "Adapters/Database/Include/Session/FakeSession.h"


namespace Allocation::Tests
{

    TEST(Services, test_returns_allocation)
    {
        Domain::OrderLine line("o1", "COMPLICATED-LAMP", 10);
        Domain::Batch batch("b1", "COMPLICATED-LAMP", 100);
        
        IRepositoryPtr repo = std::make_shared<Adapters::Repository::FakeRepository>(
            std::vector<Domain::Batch>{ batch });
        Database::ISessionPtr session = std::make_shared<Adapters::Database::FakeSession>();

        auto result = Services::Allocate(line, repo, session);
        EXPECT_EQ(result, "b1");
    }

    TEST(Services, throws_for_invalid_sku)
    {
        Domain::OrderLine line("o1", "NONEXISTENTSKU", 10);
        Domain::Batch batch("b1", "AREALSKU", 100);

        auto repo = std::make_shared<Adapters::Repository::FakeRepository>(
            std::vector<Domain::Batch>{ batch });
        auto session = std::make_shared<Adapters::Database::FakeSession>();

        try
        {
            Services::Allocate(line, repo, session);
            FAIL() << "Ожидалось исключение InvalidSku";
        }
        catch (const Services::InvalidSku& ex)
        {
            EXPECT_TRUE(std::string(ex.what()).find(
                "Недопустимый артикул NONEXISTENTSKU") != std::string::npos);
        }
        catch (...)
        {
            FAIL() << "Было выброшено неожиданное исключение";
        }
    }    
}
