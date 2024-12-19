import time
import random
import requests
from bot import Bot


def post_random_order(bot):
    pair = random.choice(bot.pairs)
    tp = random.choice(["buy", "sell"])

    pair_id = pair['pair_id']
    first = pair['first_lot_id']
    second = pair['second_lot_id']

    start_time = time.time()
    while True:
        if time.time() - start_time > 10:
            break

        quantity = round(random.uniform(0.1, 100), 3)
        price = round(random.uniform(0.001, 100), 3)
        balance = bot.get_balance()

        if (balance is None):
            return

        if tp == "buy" and balance[second] >= quantity * price:
            break
        elif tp == "sell" and balance[first] >= quantity:
            break
   
    order = {"pair_id": pair_id, "quantity": quantity, "price": price, "type": tp}
    print (order)
    bot.post_order(**order)


def main_loop(bot):
    start_time = time.time()
    while time.time() - start_time < 300:
        time.sleep(1)

        try:
            post_random_order(bot)
        except Exception as e:
            pass


def main():
    with requests.Session() as session:
        rnd_name = f'strategy_bot{random.random()}'
        bot = Bot(rnd_name, session)
        main_loop(bot)

if __name__ == "__main__":
    main()
    print("end")
