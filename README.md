# WEB CLIENT

Assignment for 2nd year Communication Protocols module.

Web Client. REST API Communication.

### The Program
This homework targets the creation and basic functions of a simple
Web Client for an online library. The user is to create an account,
login using said account, and be able to see, add and delete books.

The commands list is as follows:

- with params
* ```register```
	.params: username, password.
* ```login```
	.params: username, password.
* ```get_book```
	.params: id.
* ```add_book```
	.params: title, author, genre, publisher, page_count.
* ```delete_book```
	.params: id.

- without params
* ```enter_library```
* ```get_books```
* ```exit```
* ```logout```

## Implementation
```client.c``` - Using a loop structure, the commands are being constantly
read from standard input, until the ```exit``` command is entered.
The connection will be opened on the provided host and port, and closed
once again with every command.

```commands.c``` - For every valid command, the program will call a
different function in order to treat the different cases. The functions
that affect the cookie and/or jwt are```_login```, ```_enter_library```
and ```_logout```. Through login, a cookie is received. Same goes for
entering the online library, as the server responds with a jwt. Through the
logout process, both tokens are reset.

```macros.h``` - A list of both string and numeral macros that were used.

```helpers.c / requests.c / buffer.c``` - auxiliary libraries taken from
laboratory activity, slightly fixed to fit the context of development.

## Commands
* ```register```: From stdin, the program asks for username and password,
returning a success / error (failure) message, depending on the outcome
returned by the server's response on the POST request. The username and
password are parsed into a Json construct, using the ```parson``` lib,
which is later integrated in the message sent to the server. This is
what happens with every Json to-be-sent throughout the program.

* ```login```: Similarly to register, the program asks for username and
password, this time sending the payload in hopes of receiving a cookie
token. Depending on the server's response, the cookie is initialized or
not and the success / error message is displayed.

* ```enter_library```: Upon checking that a cookie has been received from
the server, the program will let the user access the online library, and
receive a jwt upon doing so, which will serve in validating the other funcs.

* ```get_books```: Upon checking if both a cookie and a jwt are available,
the program generates a readable list of book titles and IDs, received in
the server's response to a GET request. To be noted, if the user has not
logged in yet, the program will neither allow them to enter the library,
nor see the books list.

* ```get_book```: Similar to the command above, through this command, only
that data regarding one book is made visible to the user within one Json,
containing the title, author, genre, publisher and page_count.

* ```add_book```: Very similar to the login command, this one allows the user
to convey information about a book to the server and insert it in book format.
There are more fields which get parsed to Json, but the mechanism is the same.

* ```delete_book```: For this command, I have written another request
function in ```requests.c```, dedicated to deletion. It's mostly the same as
the GET one, having but some strings changed. Similar to get_book, an ID is
provided from stdin and the operation is transmitted to the server with
the help of a dynamic route, ending in the book's ID.

* ```logout```: The most important one, imo, as it's critical that a user
cannot access the database anymore after logging out. This is why the
program resets the fields of cookie and jwt when the command is sent, as
the tokens do not get invalidated by the server. Another fun condition to
take into account was that a user must be logged in before logging out.

* ```exit```: It is time to stop.

### Printing to Output
I have noticed there are some buffering problems upon printing more writing
prompts one after another, thus I ended up using ```fgets``` and played around
with the string tails to manage the characters better. The other small prompts
work as expected.

#### Feedback
The checker is not the best, nor the best.
I've spent more time treating useless checker errors than I spent proofreading
my own code. At the end of the day, it finally worked on new user registration.
But man, did it take the soul out of me.

Other than that, this assignment was an enjoyable one, very fluid to write,
thanks to the repetitive mechanisms behind commands (register -> login,
get_books -> get_book, login -> add_book etc.). I would consider adding more
clarifications on data sizes, data types, restrictions and hints, so that
we other students like me do not have to spend that extra hour wondering
if their macros are good as they are or not. An error I kept getting on the
checker was due to the imperiously long title name given in one of the example.
5o+ characters for a book title? Seriously? At least it was a good name, ngl.

#### References
[1] - https://pcom.pages.upb.ro/labs/lab9/lecture.html <br>
[2] - https://stackoverflow.com/ <br>
[3] - https://github.com/kgabis/parson <br>
[4] - Online Documentation

##### Copyright 2023 Popescu Cleopatra 323CA
