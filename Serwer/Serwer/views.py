from django.shortcuts import render
from django.contrib.auth.models import User

from django.utils import timezone
from django.http import HttpResponseRedirect, HttpResponse
from django.core.urlresolvers import reverse

# Create your views here.
# Like Task controller


def home(request):
  
  return render(request, 'home.html')
