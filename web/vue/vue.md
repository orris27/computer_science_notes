## 1. 开源项目
### CoreUI
```
git clone https://github.com/coreui/coreui-free-vue-admin-template.git CoreUI-Vue

# 解决chromedriver安装不了的问题
mkdir ~/.npm-global
npm config set prefix '~/.npm-global'
export PATH=~/.npm-global/bin:$PATH
NPM_CONFIG_PREFIX=~/.npm-global

npm install chromedriver
npm install

# run
npm run serve
```
