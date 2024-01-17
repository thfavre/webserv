
import sys
print("<h1>Upload CGI</h1>")
print("<p>Python version: " + sys.version + "</p>")
print("<p>sys.argv: " + str(sys.argv) + "</p>")
# #!/usr/bin/env python
# import cgi
# import os

# # Définir la limite de poids du fichier (en octets)
# limit_size = 5 * 1024 * 1024  # 5 Mo

# # Récupérer le fichier uploadé

# form =cgi.FieldStorage()
# print(form)
# uploaded_file = form['image']

# # Vérifier si un fichier a été soumis
# if uploaded_file.filename:
# 	# Vérifier la taille du fichier
# 	if uploaded_file.file and uploaded_file.file.tell() <= limit_size:
# 		# Déplacer le fichier vers le dossier d'images
# 		upload_dir = "."
# 		file_path = os.path.join(upload_dir, uploaded_file.filename)

# 		with open(file_path, 'wb') as new_file:
# 			new_file.write(uploaded_file.file.read())

# 		# Rediriger vers la page de réussite
# 		print("Content-Type: text/html")
# 		print("Location: upload.html\n")
# 	else:
# 		# Rediriger vers la page d'échec en cas de dépassement de la limite de poids
# 		print("Content-Type: text/html")
# 		print("Location: uploadfail.html\n")
# else:
# 	 # Rediriger vers la page d'échec si aucun fichier n'a été soumis
# 	print("Content-Type: text/html")
# 	print("Location: uploadfail.html\n")
