# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Task',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('depth', models.PositiveSmallIntegerField()),
                ('x', models.PositiveSmallIntegerField()),
                ('y', models.PositiveSmallIntegerField()),
                ('taskPerThread', models.PositiveSmallIntegerField()),
                ('colorR', models.PositiveSmallIntegerField()),
                ('colorG', models.PositiveSmallIntegerField()),
                ('colorB', models.PositiveSmallIntegerField()),
                ('registration_time', models.DateTimeField()),
                ('start_time', models.DateTimeField()),
                ('finish_time', models.DateTimeField()),
                ('status', models.CharField(max_length=128)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
    ]
