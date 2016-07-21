# -*- coding: utf-8 -*-
"""
Created on Wed Jul 20 19:48:04 2016

@author: darek
"""

from pymongo import MongoClient
import datetime
import asyncio
import time
#import ble_mod


class mainClass:
    temperature = 10
    
    def __init__(self):
        client = MongoClient("mongodb://ramboPower:QAZXSW1200@ds023425.mlab.com:23425/rambodb")
        db = client['rambodb']
        self.collection = db['StacjaMeteo']
    
    @asyncio.coroutine
    def get_some_data(self):
        while True:
            print(datetime.datetime.now().strftime("%Y-%m-%d %H:%M"))
            yield from asyncio.sleep(2)
            self.temperature = self.temperature + 1
            
    def update_database(self):
        while True:
            print("Update database = " + str(self.temperature))
            now = datetime.datetime.now()
            data = {
                'location': 'Kolo TV',
                'stacja_id': 1, 
                'temperature': self.temperature, 
                'humidity': 40, 
                'pm10': 0, 
                'pm25': 0,
                'timestamp': now.strftime("%Y-%m-%d %H:%M"), 
                'rssi' : -80
            }
            self.collection.insert_one(data)
            yield from asyncio.sleep(10)
        


try:
    test = mainClass()
    
    loop = asyncio.get_event_loop()
    loop.create_task(test.get_some_data())
    loop.create_task(test.update_database())
    loop.run_forever()

except:
    loop.close()

#try:
#        ble = ble_mod()
#        ble.start_scan()
#        ble.get_devices()
#except KeyboardInterrupt:
#        ble.stop_scan()
#        print("Bye")