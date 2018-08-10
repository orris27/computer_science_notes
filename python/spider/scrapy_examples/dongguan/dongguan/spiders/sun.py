# -*- coding: utf-8 -*-
import scrapy
from scrapy.linkextractors import LinkExtractor
from scrapy.spiders import CrawlSpider, Rule
from dongguan.items import *


class SunSpider(CrawlSpider):
    name = 'sun'
    allowed_domains = ['wz.sun0769.com']
    start_urls = ['http://wz.sun0769.com/index.php/question/questionType?type=4&page=0']

    rules = (
        Rule(LinkExtractor(allow=r'type=4&page=\d+'),follow=True,process_links='handle_links'),
        Rule(LinkExtractor(allow=r'question/\d+/\d+.shtml'),callback='parse_item',follow=False)
    )

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
        
