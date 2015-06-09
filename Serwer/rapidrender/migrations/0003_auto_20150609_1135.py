# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('rapidrender', '0002_task_user'),
    ]

    operations = [
        migrations.AddField(
            model_name='task',
            name='fractal_type',
            field=models.PositiveSmallIntegerField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='task',
            name='frames',
            field=models.PositiveSmallIntegerField(default=1),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='task',
            name='im',
            field=models.TextField(default='0.0'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='task',
            name='re',
            field=models.TextField(default='0.0'),
            preserve_default=False,
        ),
        migrations.AddField(
            model_name='task',
            name='speed',
            field=models.TextField(default=0),
            preserve_default=False,
        ),
    ]
