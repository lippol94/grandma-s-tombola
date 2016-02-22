# Grandma's Tombola

**Grandma's Tombola** is an academic project developed on my own for an exam I
attended. I was asked to create a distributed version of Tombola, a well-know
italian game (see [Wikipedia entry](https://en.wikipedia.org/wiki/Tombola_(raffle)))
similar to Bingo.

![Grandma's Tombola Home image](https://raw.githubusercontent.com/lippol94/grandma-s-tombola/master/ui/background_1.png)

The project is built upon two parts, a *server* application and a *client*
application. The former is supposed to be considered as a demon running on local
network or on a server online. Since it's running, any client is able to connect
to it and start playing. The server supports theoretically any number of clients
and any number of games. You can create your game room, wait for friends to join
and start playing. You can claim wins, chat with your opponents and of course, win.

Every function, data structure or type is *documented*. You can access documentation
through a convenient HTML page generated by Doxygen tool. Feel free to clone this
repository and open [documentation main page](https://github.com/lippol94/grandma-s-tombola/blob/master/doc/html/index.html).

**Grandma's Tombola** is written in C including GNOME project libraries (see *GLib*,
*GIO* and *GTK*).
