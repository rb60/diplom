CREATE TABLE IF NOT EXISTS Student (
        id INTEGER NOT NULL  PRIMARY KEY,
        name TEXT
);


CREATE TABLE IF NOT EXISTS Subject (
        id INTEGER NOT NULL PRIMARY KEY,
        name Text
);

CREATE TABLE IF NOT EXISTS Task (
        id INTEGER NOT NULL PRIMARY KEY,
        subject INTEGER,
        name TEXT,
        FOREIGN KEY(subject) REFERENCES Subject(id)
);

CREATE TABLE IF NOT EXISTS MarkValue (
        id INTEGER NOT NULL PRIMARY KEY,
        _value FLOAT
);

CREATE TABLE IF NOT EXISTS Mark(
        id INTEGER NOT NULL PRIMARY KEY,
        task INTEGER,
        student INTEGER,
        _value INTEGER,
        FOREIGN KEY(task) REFERENCES Task(id)
        FOREIGN KEY(student) REFERENCES Student(id)
        FOREIGN KEY(_value) REFERENCES MarkValue(id)
);

CREATE TABLE IF NOT EXISTS ViewTable (
        id INTEGER NOT NULL  PRIMARY KEY,
        subject INTEGER UNIQUE,
        FOREIGN KEY(subject) REFERENCES Subject(id)
);

CREATE TABLE IF NOT EXISTS ViewColumn (
        id INTEGER NOT NULL  PRIMARY KEY,
        viewTable INTEGER,
        name TEXT,
        _order INTEGER,
        task INTEGER,
        FOREIGN KEY(viewTable) REFERENCES ViewTable(id)
        FOREIGN KEY(task) REFERENCES Task(id)
);

CREATE TABLE IF NOT EXISTS ViewRow (
        id INTEGER NOT NULL  PRIMARY KEY,
        viewTable INTEGER,
        name TEXT,
        _order INTEGER,
        student INTEGER,
        FOREIGN KEY(viewTable) REFERENCES ViewTable(id)
        FOREIGN KEY(student) REFERENCES Student(id)
);

CREATE TABLE IF NOT EXISTS ViewLayer (
        id INTEGER NOT NULL  PRIMARY KEY,
        viewTable INTEGER,
        name TEXT,
        _order INTEGER,
        FOREIGN KEY(viewTable) REFERENCES ViewTable(id)
);

CREATE TABLE IF NOT EXISTS ViewCell (
        id INTEGER NOT NULL  PRIMARY KEY,
        viewColumn INTEGER,
        viewRow INTEGER,
        viewLayer INTEGER,
        viewTable INTEGER,
        _value INTEGER,
        FOREIGN KEY(viewColumn) REFERENCES ViewColumn(id)
        FOREIGN KEY(viewRow) REFERENCES ViewRow(id)
        FOREIGN KEY(viewLayer) REFERENCES ViewLayer(id)
        FOREIGN KEY(viewTable) REFERENCES ViewTable(id)
        FOREIGN KEY(_value) REFERENCES MarkValue(id)
);

CREATE TABLE IF NOT EXISTS CellGroup (
        id INTEGER NOT NULL  PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS CellToCellGroup (
        _group INTEGER,
        cell INTEGER,
        FOREIGN KEY(_group) REFERENCES CellGroup(id)
        FOREIGN KEY(cell) REFERENCES ViewCell(id)
        PRIMARY KEY(_group, cell)

);

CREATE TABLE IF NOT EXISTS Strategy (
        id INTEGER NOT NULL  PRIMARY KEY,
        name TEXT UNIQUE
);

CREATE TABLE IF NOT EXISTS CellGenerator (
        id INTEGER NOT NULL  PRIMARY KEY,
        destination INTEGER,
        _order INTEGER,
        strategy INTEGER,
        FOREIGN KEY(destination) REFERENCES ViewCell(id)
        FOREIGN KEY(strategy) REFERENCES Strategy(id)
);


CREATE TABLE IF NOT EXISTS GroupToCellGenerator (
        generator INTEGER,
        _group INTEGER,
        _order INTEGER,
        FOREIGN KEY(generator) REFERENCES CellGenerator(id)
        FOREIGN KEY(_group) REFERENCES CellGroup(id)
        PRIMARY KEY(_group, generator)

);



CREATE TABLE IF NOT EXISTS MarkGroup (
    id INTEGER NOT NULL  PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS MarkToMarkGroup (
        _group INTEGER,
        mark INTEGER,
        FOREIGN KEY(_group) REFERENCES MarkGroup(id)
        FOREIGN KEY(mark) REFERENCES MarkValue(id)
        PRIMARY KEY(_group, mark)
);

CREATE TABLE IF NOT EXISTS CalculatedMark (
        id INTEGER NOT NULL PRIMARY KEY,
        _value INTEGER,
        _order INTEGER,
        strategy INTEGER,
        FOREIGN KEY(_value) REFERENCES MarkValue(id)
        FOREIGN KEY(strategy) REFERENCES Strategy(id)

);

CREATE TABLE IF NOT EXISTS GroupToMarkCalculator (
        calculator INTEGER,
        _group INTEGER,
        _order INTEGER,
        FOREIGN KEY(calculator) REFERENCES CalculatedMark(id)
        FOREIGN KEY(_group) REFERENCES MarkGroup(id)
        PRIMARY KEY(_group, calculator)
);

