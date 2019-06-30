# Django

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

   