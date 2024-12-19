import json
import requests
import time
import threading

def load_config(filename):
    with open(filename, "r") as f:
        config = json.load(f)
    return config


def print_error(lines):
    return '\n'.join (line.decode('utf-8') for line in lines)


def request(func, url, *args, **kwargs):
    try:
        response = func(url, *args, **kwargs)
        response.raise_for_status()
        return response.json()
    except requests.exceptions.RequestException as e:
        return None


def create_user(session, url, username):
    func = getattr(session, "post")
    url += '/user'
    return request(func, url, json={"username": username})


def create_order(session, url, user_key, **kwargs):
    headers = {"X-USER-KEY": user_key}
    func = getattr(session, "post")
    url += '/order'
    return request(func, url, headers=headers, json=kwargs)


def get_balance(session, url, user_key):
    headers = {"X-USER-KEY": user_key}
    func = getattr(session, "get")
    url += '/balance'
    bs = request(func, url, headers=headers)
    return {b["lot_id"]:b["quantity"] for b in bs}

def get_order(session, url, user_key):
    headers = {"X-USER-KEY": user_key}
    func = getattr(session, "get")
    url += '/order'
    return request(func, url, headers=headers)


def get_lots(session, url):
    func = getattr(session, "get")
    url += '/balance'
    return request(func, url)


def get_pairs(session, url):
    func = getattr(session, "get")
    url += '/pair'
    return request(func, url)


def delete_order(session, url, user_key, order_id):
    headers = {"X-USER-KEY": user_key}
    func = getattr(session, "delete")
    url += '/order'
    return request(func, url, headers=headers, json={"order_id": order_id})


class Bot:
    def __init__(self, username, session, config_file="bot_config.json"):
        config = load_config(config_file)

        self.url = f"http://{config['ip']}:{config['port']}"
        self.session = session

        key = create_user(session, self.url, username)

        if key is None:
            return
        
        self.key = key["key"]

        self.lots = list(config['lots'])
        self.pairs = get_pairs(self.session, self.url)
        self.balance = get_balance(self.session, self.url, self.key)

        self.time = time.time()

    def post_order(self, **kwargs):
        return create_order(self.session, self.url, self.key, **kwargs)
    
    def lot_id(self, lot_name):
        return self.lots.index(lot_name) + 1

    def get_balance(self):
        return get_balance(self.session, self.url, self.key)

    def get_lots(self):
        return self.lots
    
    def get_pairs(self):
        return self.pairs
    
    def orders(self):
        return get_order(self.session, self.url, self.key)

    def delete_order(self, order_id):
        return delete_order(self.session, self.key, order_id)