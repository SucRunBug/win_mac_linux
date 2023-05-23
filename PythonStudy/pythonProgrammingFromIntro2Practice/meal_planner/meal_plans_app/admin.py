from django.contrib import admin

# Register your models here.
from .models import Date, Time, Meal
admin.site.register(Date)
admin.site.register(Time)
admin.site.register(Meal)
