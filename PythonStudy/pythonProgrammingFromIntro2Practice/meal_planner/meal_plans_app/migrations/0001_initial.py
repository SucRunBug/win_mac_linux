# Generated by Django 4.2.1 on 2023-05-23 09:14

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Date',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('text', models.TextField(max_length=20)),
                ('date_added', models.DateTimeField(auto_now_add=True)),
            ],
        ),
        migrations.CreateModel(
            name='Time',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('text', models.TextField(max_length=20)),
                ('date_added', models.DateTimeField(auto_now_add=True)),
                ('date', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='meal_plans_app.date')),
            ],
            options={
                'verbose_name_plural': 'times',
            },
        ),
        migrations.CreateModel(
            name='Meal',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('text', models.TextField()),
                ('date_added', models.DateTimeField(auto_now_add=True)),
                ('time', models.ForeignKey(on_delete=django.db.models.deletion.CASCADE, to='meal_plans_app.time')),
            ],
        ),
    ]