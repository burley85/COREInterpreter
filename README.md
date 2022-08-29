<h1>CORE Language Interpreter</h1>

<h3>The Language<h3>

<h5>Grammar:</h5>
    <p>&emsp;&emsp;<code>&lt;prog> ::= program &lt;decl-seq> begin &lt;stmt-seq> end | program begin &lt;stmt-seq> end</code></p>
    <p>&emsp;&emsp;<code>&lt;decl-seq> ::= &lt;decl> | &lt;decl>&lt;decl-seq> | &lt;func-decl> | &lt;func-decl>&lt;decl-seq></code></p>
    <p>&emsp;&emsp;<code>&lt;stmt-seq> ::= &lt;stmt> | &lt;stmt>&lt;stmt-seq></code></p>
    <p>&emsp;&emsp;<code>&lt;decl> ::= &lt;decl-int> | &lt;decl-ref></code></p>
    <p>&emsp;&emsp;<code>&lt;decl-int> ::= int &lt;id-list> ;</code></p>
    <p>&emsp;&emsp;<code>&lt;decl-ref> ::= ref &lt;id-list> ;</code></p>
    <p>&emsp;&emsp;<code>&lt;id-list> ::= id | id , &lt;id-list></code></p>
    <p>&emsp;&emsp;<code>&lt;func-decl> ::= id ( ref &lt;formals> ) { &lt;stmt-seq> }</code></p>
    <p>&emsp;&emsp;<code>&lt;formals> ::= id | id , &lt;formals></code></p>
    <p>&emsp;&emsp;<code>&lt;stmt> ::= &lt;assign> | &lt;if> | &lt;loop>  | &lt;out> | &lt;decl> | &lt;func-call></code></p>
    <p>&emsp;&emsp;<code>&lt;func-call> ::= begin id ( &lt;formals> ) ;</code></p>
    <p>&emsp;&emsp;<code>&lt;assign> ::= id = input ( ) ; | id = &lt;expr> ; | id = new class; | id = share id ;</code></p>
    <p>&emsp;&emsp;<code>&lt;out> ::= output ( &lt;expr> ) ;</code></p>
    <p>&emsp;&emsp;<code>&lt;if> ::= if &lt;cond> then { &lt;stmt-seq> } | if &lt;cond> then { &lt;stmt-seq> } else { &lt;stmt-seq> }</code></p>
    <p>&emsp;&emsp;<code>&lt;loop> ::= while &lt;cond> { &lt;stmt-seq> }</code></p>
    <p>&emsp;&emsp;<code>&lt;cond> ::= &lt;cmpr> | ! ( &lt;cond> ) | &lt;cmpr> or &lt;cond></code></p>
    <p>&emsp;&emsp;<code>&lt;cmpr> ::= &lt;expr> == &lt;expr> | &lt;expr> &lt; &lt;expr> | &lt;expr> &lt;= &lt;expr></code></p>
    <p>&emsp;&emsp;<code>&lt;expr> ::= &lt;term> | &lt;term> + &lt;expr> | &lt;term> – &lt;expr></code></p>
    <p>&emsp;&emsp;<code>&lt;term> ::= &lt;factor> | &lt;factor> * &lt;term></code></p>
    <p>&emsp;&emsp;<code>&lt;factor> ::= id | const | ( &lt;expr> )</code></p>

<h5>Notes:</h5>
    <p>&emsp;&emsp;Identifiers are limited to 32 characters, the first of which must be an alphabetic character,  
    and the remaining characters can alphanumeric.</p>
    <p>&emsp;&emsp;Literal constants must be in the range [0,255]; however, variables can hold any value that can fit in an <code>int</code> in C. This means
    </br>&emsp;&emsp;&emsp; that variables can only be initialized to a number in that range, but they can be incremented past 255 and decremented below 0.</p>
<h5>Undefined Behavior:</h5> 
    <p>&emsp;&emsp;Passing an int variable into a function</p>
    <p>&emsp;&emsp;Using a ref variable before initializing</p>
    <p>&emsp;&emsp;Passing the wrong number of arguments into a function</p>

<h3>Planned Features and Changes:</h3>
    <p>&emsp;&emsp;Add pointer arithmetic. The CORE language grammar will be updated to support this.
    <br>&emsp;&emsp;&emsp;(<code>id = share id ;</code> will be changed to <code>id = share id + &lt;expr> ; | id = share id - &lt;expr> ;</code>)</p>
    <p>&emsp;&emsp;Comparison expression to check if two references refer to the same integer in the heap.
    <br>&emsp;&emsp;&emsp;(<code>id == share id </code> will be added to the list of <code>&lt;cmpr></code> expressions </p>
<h3>Testing:</h3>
    <p>&emsp;&emsp;The bulk of my testing was done by writing programs in the CORE programming language that solve typical algorithm problems (Two Sum, implementing linked lists, etc.), then writing a python script to generate 100 random inputs for those problems and running the program using that input to ensure the results are as expected.</p>