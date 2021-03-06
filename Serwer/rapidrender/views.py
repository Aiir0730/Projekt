from django.shortcuts import render
from rapidrender.models import Task
from django.contrib.auth.models import User
import Serwer.middlewares

from django.utils import timezone
from django.http import HttpResponseRedirect, HttpResponse
from django.core.urlresolvers import reverse
from django.contrib.auth.decorators import login_required

import subprocess
import os
import logging
import shutil
from math import log10

import zmq
import time
import sys
from  multiprocessing import Process

#from scitools.std import movie
#from PIL import Image, ImageSequence

# Create your views here.
# Like Task controller

@login_required
def index(request):
  tasks = request.user.task_set.all()
  return render(request, 'tasks/index.html', {'tasks': tasks, 'current_user': request.user})

@login_required
def detail(request, task_id):
  task = Task.objects.get(id=task_id)

  path = os.path.dirname(os.path.realpath(__file__)) + '/../' + task_id + '.bmp'
  destpath = os.path.dirname(os.path.realpath(__file__)) + '/../static/img/' + task_id + '.bmp'

  animation_path = os.path.dirname(os.path.realpath(__file__)) + '/../' + task_id + '_' + str(task.frames-1) + '.bmp'

  if task.status == 'At work' and task.frames == 1 and os.path.isfile(path):
    os.rename(path, destpath)
    task.status = "Finished"
    task.finish_time = timezone.now()
    task.save()

  elif task.status == 'At work' and task.frames > 1 and os.path.isfile(animation_path):
    copy_animation(task.frames, task_id, os.path.dirname(os.path.realpath(__file__)))
    #movie(task_id+'_*.bmp',fps=23,output_file=task_id+'.gif')
    task.status = "Finished"
    task.finish_time = timezone.now()
    task.save()    

  return render(request, 'tasks/detail.html', {'task': task})

@login_required
def new(request):
  return render(request, 'tasks/new.html', {})

@login_required
def create(request):
  color = __color_helper(request.POST['color'])

  newtask = Task(depth = request.POST['depth'],
                 x     = request.POST['x'],
                 y     = request.POST['y'],
                 taskPerThread = request.POST['taskPerThread'],
                 colorR = color['r'],
                 colorG = color['g'],
                 colorB = color['b'],
                 registration_time = timezone.now(),
                 start_time = timezone.now(),
                 finish_time = timezone.now(),
                 status = "Registered",
                 user_id = request.user.id,
                 fractal_type = request.POST['fractal_type'],
                 re = request.POST['re'],
                 im = request.POST['im'],
                 frames = request.POST['frames'],
                 speed = request.POST['speed'],
                 )

  newtask.save()
  return HttpResponseRedirect(reverse('detail', args=(newtask.id,)))

@login_required
def update(request, task_id):
  task = Task.objects.get(id=task_id)
  if request.method == 'GET':
    color = __color_to_html(task)
    render(request, 'tasks/update.html', {'task': task, 'html_color': color})
  else:
    task.depth = request.POST['depth']
    task.x     = request.POST['x']
    task.y     = request.POST['y']
    task.taskPerThread = request.POST['taskPerThread']

    color = __color_helper(request.POST['color'])
    task.colorR = color['r']
    task.colorG = color['g']
    task.colorB = color['b']
    task.fractal_type = request.POST['fractal_type'],
    task.re = request.POST['re'],
    task.im = request.POST['im'],
    task.frames = request.POST['frames'],
    task.speed = request.POST['speed'],

    task.registration_time = timezone.now()
    task.start_time = timezone.now()
    task.finish_time = timezone.now()
    task.status = "Updated"

    task.save()
    return HttpResponseRedirect(reverse('detail', args=(task.id,)))

@login_required
def delete(request, task_id):
  task = Task.objects.get(id=task_id)
  task.delete()

  return HttpResponseRedirect(reverse('index'))

@login_required
def start(request, task_id):
  task = Task.objects.get(id=task_id)

  Serwer.middlewares.SocketMiddleware.statushash[task_id] = (0,0)

  #mpipath = os.path.dirname(os.path.realpath(__file__)) + '/../../Mandelbrot_MPI/mpi'
  mpipath = '/mpi/mpi'
  filename = 'mpirun'
  arg1 = '-f'
  #arg2 = os.path.dirname(os.path.realpath(__file__)) + '/../../Mandelbrot_MPI/hostfile'
  arg2 = '/mpi/hostfile'
  #arg1 = '-n'
  #arg2 = '5'
  arg3 = str(mpipath)
  arg4 = str(task.depth)
  arg5 = str(task.taskPerThread)
  arg6 = str(task.x)
  arg7 = str(task.y)
  arg8 = str(task.colorR)
  arg9 = str(task.colorG)
  arg10 = str(task.colorB)
  arg_id = str(task_id)
  arg11 = str(task.fractal_type) #typ fraktala   
  arg12 = task.re
  arg13 = task.im
  arg14 = str(task.frames)
  arg15 = str(task.speed)

  #print("Debug %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s ",
  #  (filename, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg_id, arg11, arg12, arg13, arg14, arg15))

  #server_port = 9999
  #Process(target=server, args=(server_port,)).start()

  print("Przed")
  subprocess.Popen([filename, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg_id, arg11, arg12, arg13, arg14, arg15]) 
  print("PO")
  #logger = logging.getLogger(__name__)
  #logger.error(filename)

  task.status = "At work"
  task.start_time = timezone.now()
  task.save()

  return HttpResponseRedirect(reverse('detail', args=(task.id,)))

def status(request, task_id):


  info = Serwer.middlewares.SocketMiddleware.statushash[task_id]
  #path = os.path.dirname(os.path.realpath(__file__)) + '/../../Mandelbrot_MPI/' + task_id + '.txt'
  #fifo = open(path, 'r+')
  #info = fifo.read()
  #fifo.write(info)
  #fifo.close()

  return HttpResponse(info, content_type='text/plain')

def finish(request, task_id):
  task = Task.objects.get(id=task_id)
  task.finish_time = timezone.now()
  task.status = "Finished"
  task.save()

  #path = os.path.dirname(os.path.realpath(__file__)) + '/../../Mandelbrot_MPI/' + task_id + '.txt'
  #os.remove(path) #remove fifo

  #copy imagefile to statics

  return HttpResponseRedirect(reverse('detail', args=(task.id,)))

@login_required
def reset(request, task_id):
  task = Task.objects.get(id=task_id)
  task.status = "Registered"
  task.save()

  return HttpResponseRedirect(reverse('detail', args=(task.id,)))

###private###

def __color_helper(html_color):
  color_string = "0x" + html_color[1:]
  color_value = int(color_string, 16)
  r = color_value >> 16
  g = (color_value >> 8) & 255
  b = color_value & 255
  return {'r': r,'g': g,'b': b}

def __color_to_html(task):
  value = (task.colorR << 16) | (task.colorG << 8) | task.colorB;
  return "#" + hex(value)[2:]

def copy_animation(frames, task_id, dirname):
  n = int(log10(2*frames))+1
  for i in range(frames):
    path = dirname + '/../' + task_id + '_' + str(i) + '.bmp'
    destpath = dirname + '/../static/img/' + task_id + '_' + str(i).zfill(n) + '.bmp'
    returnpath = dirname + '/../static/img/' + task_id + '_' + str(2*frames - i - 1).zfill(n) + '.bmp'
    os.rename(path, destpath)
    shutil.copyfile(destpath, returnpath)


  srcpath = dirname + '/../static/img/'+task_id+'_*.bmp'
  avipath = dirname + '/../static/output/avi'+task_id+'.avi'
  gifpath = dirname + '/../static/output/gif'+task_id+'.gif'
  os.system('mencoder "mf://'+srcpath+'" -mf type=bmp:fps=23 -ovc lavc -o '+avipath+' -lavcopts vcodec=mpeg4')
  os.system('mplayer "mf://'+srcpath+'" -mf w=2000:h=2000:type=bmp -vf scale=500:500 -vo gif89a:fps=23:output='+gifpath)

def server(port="9999"):
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:%s" % port)
    print("Running server on port: ", port)
    # serves only 5 request and dies
    while True:
        # Wait for next request from client
        message = socket.recv()
        (task_id, progress, done) = message.split(":")
        print("Received request #%s: %s" % (reqnum, message))
        #socket.send("World from %s" % port)
        if done == '1':
          break;
