# Django

## Start a project
```
django-admin startproject object_tracking_robot
```
## Run Django
```
python manage.py runserver
```

## Migrations

1. Write tables codes in `model.py` 

```python
class Card(models.Model):
   name=models.CharField(max_length=20)
   department=models.CharField(max_length=40)
   type=models.BooleanField()
   def __str__(self):
       return str(self.pk)
   class Meta():
       db_table='card'
   @classmethod
   def create(cls,name,department,type):
       c=Card()
       c.name=name
       c.department=department
       c.type=type
       return c
```

   

2. Regist model in `admin.py`

   ```python
   # Register your models here.
   admin.site.register(Book,BookAdmin)
   admin.site.register(Card)
   admin.site.register(BorrowRecord)
   admin.site.register(Manager)
   ```

3. Migrate

   ```shell
   python manage.py  makemigrations
   python manage.py  migrate
   ```
## Query Set
1. query all the data
```
books = list(Book.objects.all())
for book in books:
    print(book.id)
    print(book.title)
    print(book.photo)
```

## views -> template
```
err_name = 123 # python object. In fact, there is no need to dump for simple type such as list
book_ids = [1, 2]
context = {'title':'首页','err_name':json.dumps(err_name), 'book_ids':book_ids}

# xx.html
<div class="row">
   {{ err_name }}
</div>

{% for book_id in book_ids %}
    <h1>{{ book_id }}</h1>
{% endfor %}
```
