from django.urls import path

from . import views

app_name = 'pizza_app'

urlpatterns = [
    path('', views.index, name='index'),
    path('pizzas/', views.pizzas, name='pizzas'),
    path('pizza/<int:pizza_id>/', views.pizza, name='pizza')
]