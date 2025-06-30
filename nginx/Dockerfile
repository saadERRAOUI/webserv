FROM debian:buster

RUN		mkdir -p /var/run/nginx

RUN apt update -y && apt upgrade -y && apt install -y \
	nginx


COPY nginx.conf /etc/nginx/conf.d

RUN		mkdir -p /var/run/nginx
RUN		chmod -R 775 /var/www/html
RUN		chown -R www-data:www-data /var/www/html

CMD		["nginx" , "-g" ,"daemon off;"]
