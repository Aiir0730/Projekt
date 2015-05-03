from django.shortcuts import render
from rapidrender.models import Task

from django.utils import timezone
from django.http import HttpResponseRedirect, HttpResponse
from django.core.urlresolvers import reverse

# Create your views here.
# Like Task controller

def index(request):
  tasks = Task.objects.all()
  return render(request, 'tasks/index.html', {'tasks': tasks})

def detail(request, task_id):
  task = Task.objects.get(id=task_id)
  return render(request, 'tasks/detail.html', {'task': task})

def new(request):
  return render(request, 'tasks/new.html', {})

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
                 )

  newtask.save()
  return HttpResponseRedirect(reverse('detail', args=(newtask.id,)))

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

def delete(request, task_id):
  task = Task.objects.get(id=task_id)
  task.delete()

  return HttpResponseRedirect('index')

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
