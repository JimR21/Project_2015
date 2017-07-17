# ACM Sigmod 2015 Programming Contest
Conflict detection system in database queries

Course: Computer Software Development

Professor: Dr. Y. Ioannidis

##  Info
The ACID principle, mandating atomicity, consistency, isolation and durability is one of the cornerstones of relational database management systems. In order to achieve the third principle, isolation, concurrent reads and writes must be handled carefully.

In practice, transaction processing is often done in an optimistic fashion, where queries and statements are processed first, and only at the commit point the system validates if there was a conflict between readers and writers.

In this challenge, we simulate this in a slightly simplified way: The system processes already executed transactions and only needs to check efficiently whether concurrent queries conflict with them. So, in short: <b>we issue a list of insert and delete statements and our program needs to figure out whether given predicates match the inserted or deleted data.<b>

Check out more >>> <http://db.in.tum.de/sigmod15contest/task.html>

## Optimizations
Memory and time optimizations were implemented on 3 levels. You can see the implementation decisions and more details here >>> <https://github.com/JimR21/Project_2015/blob/master/Project2016.pdf>
