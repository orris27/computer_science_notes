#
# See documentation in:
# https://doc.scrapy.org/en/latest/topics/items.html

import scrapy
class TencentItem(scrapy.Item):
    # define the fields for your item here like:
    name = scrapy.Field()
    cat=scrapy.Field()
    people=scrapy.Field()
    place=scrapy.Field()
    publish=scrapy.Field()

