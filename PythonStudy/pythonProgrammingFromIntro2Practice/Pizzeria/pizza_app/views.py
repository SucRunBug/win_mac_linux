from django.shortcuts import render
from .models import Pizza

# Create your views here.
def index(request):
    return render(request, 'pizza_app/index.html')

def pizzas(request):
    pizzas = Pizza.objects.order_by('date_added')
    context = {'pizzas': pizzas}
    return render(request, 'pizza_app/pizzas.html', context)