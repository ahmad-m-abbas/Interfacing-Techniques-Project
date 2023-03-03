#include<math.h>
use name std;
int i, a, b, c;
int arr[] = { 12, 7, 2};
void func() {
  if (a == i) { a &= i;}
  if (arr[i/3] < b) { b &= 2; }
  c = std::max(a, b);
}
int main() {

  a = arr[2];
  b = 3;
  i = 9;
  for (; i > 0; i -= 3) {
    func();
  }
  return 0;
}