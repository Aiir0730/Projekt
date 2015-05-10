from django.shortcuts import render
from django.contrib.auth.models import User
from django.contrib.auth.decorators import login_required
from django.contrib.auth import authenticate, login, logout

from django.http import HttpResponseRedirect, HttpResponse
from django.core.urlresolvers import reverse

# Create your views here.

def login_action(request):
  if request.method == 'GET':
    return render(request, 'users/login.html', {}) 
  else:
    username = request.POST['username']
    password = request.POST['password']
    user = authenticate(username=username, password=password)
    if user is not None:
      if user.is_active:
        login(request, user)
        return HttpResponseRedirect(reverse('index'))
      else:
        return render(request, 'users/error.html', {'error': 'Disabled account'})
    else:
      return render(request, 'users/error.html', {'error': 'Invalid login or password'})

@login_required
def logout_action(request):
  logout(request)
  return render(request, 'users/login.html', {})

def register_action(request):
  if request.method == 'GET':
    return render(request, 'users/register.html', {})  
  else:
    user = User.objects.create_user(request.POST['username'],
      request.POST['email'], request.POST['password'])
    user.save()
    return HttpResponseRedirect(reverse('index'))

