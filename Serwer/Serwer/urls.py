from django.conf.urls import patterns, include, url
from django.contrib import admin
from Serwer import views

urlpatterns = patterns('',
    # Examples:
    # url(r'^blog/', include('blog.urls')),
    url(r'^$', views.home),
    url(r'^admin/', include(admin.site.urls)),
    url(r'^tasks/', include('rapidrender.urls')),
    url(r'^users/', include('users.urls')),
)
