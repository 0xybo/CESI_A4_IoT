# Dashboard

À cause de l'incompatibilité des images Docker avec l'architecture ARM du Raspberry PI, nous avons du abandonner l'utilisation de Docker et installer les solutions directement sur Raspbian OS.

Les solutions en question sont :

- NodeRED pour la création d'une sorte de serveur, la récupération des données depuis le port série. Une arduino avec un module ZigBee a été utilisé pour simuler une clé USB ZigBee que nous n'avions pas.
- InfluxDB pour le stockage des données de statistiques remontées par la carte Arduino des capteurs/LED et pour faire la logique côté serveur.
- Grafana pour l'affichage de représentations graphiques à partir des données stockées en base de données.

Nous avions prévu d'ajouter une interface graphique de configuration pour la base de données via Chronograf ou InfluxDB V2, cependant nous avons finalement installé InfluxDB v1 et avons manipulé la base de données depuis le CLI.
