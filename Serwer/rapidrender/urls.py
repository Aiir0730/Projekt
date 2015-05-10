from django.conf.urls import patterns, url

from rapidrender import views

urlpatterns = patterns('',
    url(r'^$', views.index),
    url(r'^new/$', views.new, name='new'),
    url(r'^(?P<task_id>[0-9]+)/$', views.detail, name='detail'),
    url(r'^create/$', views.create, name='create'),
    url(r'^(?P<task_id>[0-9]+)/update/$', views.update, name='update'),
    url(r'^(?P<task_id>[0-9]+)/delete/$', views.delete, name='delete'),
    url(r'^(?P<task_id>[0-9]+)/start/$', views.start, name='start'),
)
