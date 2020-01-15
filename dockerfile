FROM nginx
COPY build /usr/share/nginx/html
COPY default.conf /etc/nginx/conf.d/
COPY nginx.crt /etc/ssl/
COPY nginx.key /etc/ssl