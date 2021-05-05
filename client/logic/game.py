import storage
import requests
import json
from flask import request
import threading


def handle():
   storage.add_response_handler('connect', on_game_connect)
   
   storage.add_response_handler('connection-count-update', on_connection_count_update)
   storage.add_response_handler('ready-count-update', on_ready_count_update)
   
   storage.add_response_handler('rotate', on_rotate)
   storage.add_response_handler('hover', on_hover)
   storage.add_response_handler('unhover', on_unhover)
   storage.add_response_handler('start-water', on_start_water)
   
   storage.add_response_handler('start', on_game_start)
   storage.add_response_handler('win', on_win)
   

def on_game_connect(req):
   print('On connect: thread is', threading.get_ident())
   
   print('On connect: Post')
   connection_count = req['connection_count']
   storage.connetion_count = connection_count
   
   storage.add_task('setConnectionCount', connection_count)
   return '{"status": "OK"}'

def on_game_start(req):
   storage.swap['session_id'] = req['session_id']
   storage.swap['field'] = req['field']
   storage.swap['players'] = req['players']
   
   storage.add_task('loadPage', 'game')
   return '{"status": "OK"}'

def on_win(req):
   if int(req['winner']) == storage.id:
      message = 'Вы виграли!! За победу вы получили 10 000 валюты, посетите магазин ;)'
   else:
      message = 'Вы проиграли. Победитель получил 10 000 валюты в качестве вознаграждения. Побеждайте, чтобы открывать скины ;)'
   storage.add_task('showWin', message)
   
   return '{"status": "OK"}'



def on_ready_count_update(req):
   storage.add_task('setReadyCount', req['count'])
   return '{"status": "OK"}'
def on_connection_count_update(req):
   storage.connection_count = int(req['count'])
   storage.add_task('setConnectionCount', req['count'])
   return '{"status": "OK"}'

def on_rotate(req):
   if req['sender'] == storage.id:
      return
   storage.add_task('makeStepXY', req['x'], req['y'])
   return '{"status": "OK"}'
def on_hover(req):
   if req['sender'] == storage.id:
      return
   storage.add_task('forceHover', req['x'], req['y'])
   return '{"status": "OK"}'
def on_unhover(req):
   if req['sender'] == storage.id:
      return
   storage.add_task('forceUnhover', req['x'], req['y'])
   return '{"status": "OK"}'
def on_start_water(req):
   storage.add_task('startWater')
   return '{"status": "OK"}'


def get_session_id(callback):
   callback.Call(storage.swap['session_id'])
def get_user_host(callback):
   callback.Call(f'{storage.host}:{storage.port}')
def get_game_data(callback):
   print(storage)
   callback.Call({
      'session_id': storage.swap['session_id'],
      'players': storage.swap['players'],
      'field': storage.swap['field'],
   })
   
def get_connection_count(callback):
   callback.Call(storage.connection_count)
def get_ready_count(callback):
   res = storage.gameload('get-ready-count', {})
   callback.Call(res['count'])


def send_ready():
   storage.gameload('ready', {})
def send_unready():
   storage.gameload('unready', {})

def send_rotate(x, y):
   storage.gameload('rotate', {'x': x, 'y': y, 'sender': storage.id})
def send_hover(x, y):
   storage.gameload('hover', {'x': x, 'y': y, 'sender': storage.id})
def send_unhover(x, y):
   storage.gameload('unhover', {'x': x, 'y': y, 'sender': storage.id})
def send_start_water():
   storage.gameload('start-water', {})
   
def send_disconnect():
   storage.gameload('disconnect', {})
   storage.connection_process.terminate()
   

get_task_counter = 0
def get_tasks(callback):
   global get_task_counter
   get_task_counter += 1
   tasks = storage.get_all_tasks()
   if len(tasks) > 0:
      print('getTasks: some tasks', '; '.join([f'{t[0]}(' + ', '.join(map(str, t[1])) + ')' for t in tasks]))
      callback.Call(tasks)
   else:
      pass
      #print('getTasks: queue is empty ', get_task_counter)
   

handle()