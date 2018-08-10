# -*- coding: utf-8 -*-
import scrapy
from scrapy.linkextractors import LinkExtractor
from scrapy.spiders import CrawlSpider, Rule
from tencent1.items import *


class TencentspiderSpider(CrawlSpider):
    name = 'tencentSpider'
    allowed_domains = ['hr.tencent.com']
    start_urls = ['https://hr.tencent.com/position.php?&start=0']

    rules = (
        Rule(LinkExtractor(allow='start=\d+'),callback='parse_item',follow=True),
    )

    def parse_item(self, response):
        block_list=response.xpath('//tr[@class="even"]|//tr[@class="odd"]')
        for block in block_list:
            item=TencentItem()
            item['name']=block.xpath('.//a/text()').extract()[0]
            item['cat']=block.xpath('./td[2]/text()').extract()
            item['people']=block.xpath('./td[3]/text()').extract()[0]
            item['place']=block.xpath('./td[4]/text()').extract()[0]
            item['publish']=block.xpath('./td[5]/text()').extract()[0]
            yield item
        return item
