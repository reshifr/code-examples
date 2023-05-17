g++ \
  -std=c++20 -Ofast \
  brute.cpp -o /tmp/brute_b2b74b2c345b8dd2feda2720e3ae00c0 \
  -lcrypto++ -lpthread

if [ "$1" == -d ]; then
  valgrind --leak-check=full --show-leak-kinds=all \
    /tmp/brute_b2b74b2c345b8dd2feda2720e3ae00c0
else
  /tmp/brute_b2b74b2c345b8dd2feda2720e3ae00c0
  rm -rf /tmp/brute_b2b74b2c345b8dd2feda2720e3ae00c0
fi