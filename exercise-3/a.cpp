#include <optional>
#include <string>

using namespace std;

int main() {
  optional<string> x = optional<string>("Hello");
  x = "abc";
  optional<string> y = nullopt;
}