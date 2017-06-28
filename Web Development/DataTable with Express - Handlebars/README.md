- Back End uses JavaScript with Express & Handlebars. A MySQL database was connected to for the project.
- Front End used HTML, CSS, and JavaScript
- After page is loaded with data for the first time, any subsequent update in the data table was sent
    to the front end, this data being only the values. The front end would have the browser make changes to the table, 
    allowing for the server to transmit less data. In theory this would allow it to respond faster to changes from
    many clients since it has less to send to each client, letting clients pick up more of the work load.
