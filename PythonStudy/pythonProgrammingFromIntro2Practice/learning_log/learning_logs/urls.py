"""define the URL model of learning_logs"""

from django.urls import path

from . import views 

app_name = 'learning_logs'  # make django clarify clear the file urls.py
urlpatterns = [ # including the request page
    # Home page
    path('', views.index, name='index'),   
    # '' means 'localhost:8000/', it is a basic URL
    # The second parameter means call a function in view.py
    # The third parameter is the URL model name, we can reference it in other palce

    path('topics/', views.topics, name='topics'),
    # 特定主题的详细页面
    path('topics/<int:topic_id>/', views.topic, name='topic'),
]