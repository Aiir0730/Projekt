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
  if task.status == 'At work' and os.path.isfile(path):
    os.rename(path, destpath)
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

  mpipath = os.path.dirname(os.path.realpath(__file__)) + '/../../Mandelbrot_MPI/mpi'
  filename = 'mpirun'
  arg1 = '-n'
  arg2 = '5'
  arg3 = str(mpipath)
  arg4 = str(task.depth)
  arg5 = str(task.taskPerThread)
  arg6 = str(task.x)
  arg7 = str(task.y)
  arg8 = str(task.colorR)
  arg9 = str(task.colorG)
  arg10 = str(task.colorB)
  arg_id = str(task_id)

  subprocess.call([filename, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg_id]) 
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
