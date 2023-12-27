#!/usr/bin/env python3

import cgi
import os

form = cgi.FieldStorage()
print("Content-type: text/html\n")
print('<html><head>')
print('<meta charset="UTF-8">')
print('<title>CGI Response Python</title>')
print('<style>')
print('.navbar .active > a {')
print('    background-color: #343a40;')
print('        color: white;')
print('}')
print('</style>')
print('<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-KK94CHFLLe+nY2dmCWGMq91rCGa5gtU4mk92HdvYe+M/SXH301p5ILy+dN9+nJOZ" crossorigin="anonymous">')
print('</head>')
print('<body>')
print('<nav class="navbar navbar-expand navbar-dark bg-dark">')
print('<a class="navbar-brand" href="#">webserv42</a>')
print('<ul class="navbar-nav">')
print('<li class="nav-item">')
print('<a class="nav-link" href=../index.html>Accueil</a>')
print('</li>')
print('<li class="nav-item">')
print('<a class="nav-link" href="../televerser.html">Téléverser</a>')
print('</li>')
print('<li class="nav-item">')
print('<a class="nav-link" href="../team.html">Notre équipe</a>')
print('</li>')
print('<li class="nav-item">')
print('<a class="nav-link" href="../cgi.html">CGI</a>')
print('</li>')
print('</ul>')
print('</nav>')
print("</head><body>")
print("<h1>Test CGI</h1>")
print("<p>Hello World!</p>")
if "name" in form:
    print("<p>Your name is: {}</p>".format(form["name"].value))
if "login" in form:
    print("<p>Your login is: {}</p>".format(form["login"].value))

print("<h3>Environments variables:</h3>")
for key, value in os.environ.items():
    print(f"<li>{key}: {value}</li>")


print("</body></html>")
