from django.urls import path

from . import views

app_name = 'pizza_app'

urlpatterns = [
    path('', views.index, name='index')
]