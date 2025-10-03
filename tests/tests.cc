#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}

TEST_CASE("RegisterAccount: Duplicate account should throw", "[reg-1]") {
  Atm atm;
  atm.RegisterAccount(11111111, 2222, "Alice", 100);
  REQUIRE_THROWS_AS(atm.RegisterAccount(11111111, 2222, "Alice", 200), std::invalid_argument);
}

TEST_CASE("WithdrawCash: Negative withdrawal should throw", "[with-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 500);
  REQUIRE_THROWS_AS(atm.WithdrawCash(12345678, 1234, -50), std::invalid_argument);
}

TEST_CASE("WithdrawCash: Overdraft should throw runtime_error", "[with-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 100);
  REQUIRE_THROWS_AS(atm.WithdrawCash(12345678, 1234, 200), std::runtime_error);
}

TEST_CASE("WithdrawCash: Invalid credentials should throw", "[with-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 100);
  REQUIRE_THROWS_AS(atm.WithdrawCash(12345678, 9999, 50), std::invalid_argument);
}

TEST_CASE("DepositCash: Negative deposit should throw", "[dep-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300);
  REQUIRE_THROWS_AS(atm.DepositCash(12345678, 1234, -100), std::invalid_argument);
}

TEST_CASE("DepositCash: Valid deposit updates balance", "[dep-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300);
  atm.DepositCash(12345678, 1234, 200);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts[{12345678, 1234}].balance == 500);
}

TEST_CASE("DepositCash: Invalid credentials should throw", "[dep-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300);
  REQUIRE_THROWS_AS(atm.DepositCash(87654321, 4321, 100), std::invalid_argument);
}

TEST_CASE("PrintLedger: Nonexistent account should throw", "[led-1]") {
  Atm atm;
  REQUIRE_THROWS_AS(atm.PrintLedger("./ledger.txt", 11111111, 2222), std::invalid_argument);
}

TEST_CASE("Transactions: Ledger should contain both deposits and withdrawals", "[led-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 1000);
  atm.DepositCash(12345678, 1234, 500);
  atm.WithdrawCash(12345678, 1234, 200);
  atm.PrintLedger("./ledger_test.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./expected_ledger.txt", "./ledger_test.txt"));
}

TEST_CASE("CheckBalance: Invalid credentials should throw", "[bal-1]") {
  Atm atm;
  atm.RegisterAccount(11111111, 2222, "Alice", 100);
  REQUIRE_THROWS_AS(atm.CheckBalance(11111111, 9999), std::invalid_argument);
}

TEST_CASE("CheckBalance: Valid credentials return correct balance", "[bal-2]") {
  Atm atm;
  atm.RegisterAccount(11111111, 2222, "Alice", 250.75);
  REQUIRE(atm.CheckBalance(11111111, 2222) == 250.75);
}

