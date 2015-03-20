from django.conf.urls import patterns, url

from rapidrender import views

urlpatterns = patterns('',
    url(r'^$', views.index, name='index'),
)
