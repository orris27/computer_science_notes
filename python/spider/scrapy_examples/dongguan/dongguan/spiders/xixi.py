# -*- coding: utf-8 -*-
import scrapy
from scrapy.linkextractors import LinkExtractor
from scrapy.spiders import CrawlSpider, Rule
from dongguan.items import *


class SunSpider(scrapy.Spider):
    name = 'xixi'
    allowed_domains = ['wz.sun0769.com']
    url ="http://wz.sun0769.com/index.php/question/questionType?type=4&page="
    offset=0
    start_urls = [url+str(offset)]

    def parse(self,response):
        linklist=response.xpath('//a[@class="news14"]/@href').extract()
        for link in linklist:
            yield scrapy.Request(link,callback=self.parse_item)
        if self.offset<90480:
            self.offset+=30
            yield scrapy.Request(self.url+str(self.offset),callback=self.parse)

    def parse_item(self, response):
        s=response.xpath('//div[@class="pagecenter p3"]//strong[@class="tgray14"]/text()').extract()[0]
        item=DongguanItem()
        item['title']=s.split('：')[-1].split(' ')[0]
        item['number']=s.split(':')[-1].split(' ')[0]
        res=response.xpath('//div[@class="c1 text14_2"]/div[@class="contentext"]/text()').extract()
        if len(res)==0:
            item['content']="".join(response.xpath('//div[@class="c1 text14_2"]/text()').extract())
        else:
            item['content']="".join(res)

        item['url']=response.url
        yield item

    def handle_links(self,links):
        for link in links:
            print(link.url)
        return links
        
