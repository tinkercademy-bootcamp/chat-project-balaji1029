#include <optional>
#include <string>

using namespace std;

int main() {
  optional<string> x = optional<string>("Hello");
  optional<string> y = nullopt;
}