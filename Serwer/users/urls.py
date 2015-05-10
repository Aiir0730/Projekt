from django.conf.urls import patterns, url

from users import views

urlpatterns = patterns('',
    url(r'^login/$', views.login_action, name='login'),
    url(r'^logout/$', views.logout_action, name='logout'),
    url(r'^register/$', views.register_action, name='register'),
)
