#!/usr/bin/env python
import cgi
import os

form = cgi.FieldStorage()

# Récupérer l'ID de l'image à supprimer
image_id = form.getvalue('imageId')

# Chemin du dossier d'images
upload_dir = "../images/"

# Chemin complet du fichier à supprimer
file_path = os.path.join(upload_dir, image_id)

# Vérifier si le fichier existe avant de le supprimer
if os.path.exists(file_path):
	# Supprimer le fichier
	os.remove(file_path)

	# Rediriger vers la page de réussite
	print("Content-Type: text/html")
	print("Location: delete.html\n")
else:
	# Rediriger vers la page d'échec si le fichier n'existe pas
	print("Content-Type: text/html")
	print("Location: deletefail.html\n")
