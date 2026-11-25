-- @Block
-- Create Tables --

-- Books
CREATE TABLE books (
    book_id INT PRIMARY KEY,
    title VARCHAR(255),
    genre VARCHAR(255),
    price DECIMAL(6, 2),
    stock INT
);

-- Customers
CREATE TABLE customers (
    customer_id INT PRIMARY KEY,
    name VARCHAR(255),
    city VARCHAR(255),
    signup_date DATE
);

-- Orders
CREATE TABLE orders (
    order_id INT PRIMARY KEY,
    customer_id INT,
    book_id INT,
    quantity INT,
    order_date DATE,
    FOREIGN KEY (customer_id) REFERENCES customers(customer_id),
    FOREIGN KEY (book_id) REFERENCES books(book_id)
);

-- Marketing Spend
CREATE TABLE marketing_spend (
    spend_id INT PRIMARY KEY,
    customer_id INT,
    spend_amount DECIMAL(7, 2),
    FOREIGN KEY (customer_id) REFERENCES customers(customer_id)
);


-- @Block
-- Insert Data --

-- Books
INSERT INTO books VALUES
    (1, 'Data Science 101', 'Education', 29.99, 100),
    (2, 'The Art of SQL', 'Technology', 34.50, 50),
    (3, 'Mystery at the Bookstore', 'Fiction', 15.00, 20),
    (4, 'Learn Python the Hard Way', 'Education', 40.00, 30),
    (5, 'Fantasy World Chronicles', 'Fantasy', 22.50, 10);

-- Customers
INSERT INTO customers VALUES
    (1, 'Alice', 'New York', '2023-01-10'),
    (2, 'Bob', 'San Francisco', '2023-03-15'),
    (3, 'Charlie', 'Austin', '2023-06-20'),
    (4, 'Diana', 'New York', '2024-01-10'),
    (5, 'Evan', 'Chicago', '2024-04-05');

-- Orders
INSERT INTO orders VALUES
    (1, 1, 1, 2, '2024-06-01'),
    (2, 2, 2, 1, '2024-06-02'),
    (3, 1, 3, 1, '2024-06-03'),
    (4, 3, 1, 3, '2024-06-04'),
    (5, 4, 5, 2, '2024-06-04'),
    (6, 5, 2, 2, '2024-06-05'),
    (7, 2, 4, 1, '2024-06-05'),
    (8, 1, 1, 1, '2024-06-06');

-- Marketing Spend
INSERT INTO marketing_spend VALUES
    (1, 1, 50.00),
    (2, 2, 75.00),
    (3, 3, 40.00),
    (4, 4, 60.00),
    (5, 5, 35.00);

-- @Block
-- Revenue Per Book --

SELECT 
    b.title,
    SUM(o.quantity) AS number_sold,
    SUM(o.quantity * b.price) AS total_revenue
FROM orders o
JOIN books b ON o.book_id = b.book_id
GROUP BY b.title
ORDER BY total_revenue DESC;

-- @Block
-- Inventory Alerts --

SELECT 
    title, stock
FROM books
WHERE stock < 15;

-- @Block
-- Top Customers --

WITH customer_metrics AS (
    SELECT
        c.customer_id,
        c.name,
        MAX(o.order_date) AS last_order,
        COUNT(o.order_id) AS frequency,
        SUM(o.quantity * b.price) AS monetary
    FROM customers c
    JOIN orders o ON c.customer_id = o.customer_id
    JOIN books b ON o.book_id = b.book_id
    GROUP BY c.customer_id
)
SELECT *,
       DATEDIFF('2024-07-01', last_order) AS recency_days
FROM customer_metrics;

-- @Block
-- Profit per Customer --

WITH customer_metrics AS (
    SELECT
        c.customer_id,
        c.name,
        SUM(o.quantity * b.price) AS monetary
    FROM customers c
    JOIN orders o ON c.customer_id = o.customer_id
    JOIN books b ON o.book_id = b.book_id
    GROUP BY c.customer_id
)
SELECT
    cm.*,
    ms.spend_amount,
    cm.monetary - ms.spend_amount AS profit
FROM customer_metrics cm
JOIN marketing_spend ms ON cm.customer_id = ms.customer_id;

-- @Block
-- Monthly Sales Trends --

SELECT 
    DATE_FORMAT(order_date, '%Y-%m') AS month,
    SUM(quantity * b.price) AS total_revenue
FROM orders o
JOIN books b ON o.book_id = b.book_id
GROUP BY month
ORDER BY month;

-- @Block
-- Customer Loyalty --

SELECT 
    c.customer_id,
    c.name,
    COUNT(DISTINCT o.order_id) AS total_orders
FROM customers c
JOIN orders o ON c.customer_id = o.customer_id
GROUP BY c.customer_id
HAVING total_orders > 1;

-- @Block
-- Average Revenue per Order

SELECT 
    ROUND(SUM(quantity * b.price) / COUNT(DISTINCT o.order_id), 2) AS avg_order_value
FROM orders o
JOIN books b ON o.book_id = b.book_id;

-- @Block
-- Books Sold Together --

SELECT 
    o1.book_id AS book_1,
    o2.book_id AS book_2,
    COUNT(*) AS times_bought_together
FROM orders o1
JOIN orders o2 
  ON o1.customer_id = o2.customer_id AND o1.order_id != o2.order_id
WHERE o1.book_id < o2.book_id -- Prevents double counting OR self-matching
GROUP BY book_1, book_2
ORDER BY times_bought_together DESC
LIMIT 10;

-- @Block
-- Old Customers --

-- 'Current Date' hard coded to '2025-07-01'
-- Use CURDATE() to get an automatic 'Current Date'

SELECT 
    c.customer_id,
    c.name,
    MAX(o.order_date) AS last_purchase,
    DATEDIFF('2025-07-01', MAX(o.order_date)) AS days_since_last_purchase
FROM customers c
JOIN orders o 
    ON c.customer_id = o.customer_id
GROUP BY c.customer_id
HAVING days_since_last_purchase > 365;

-- @Block
-- Reset --

-- DROP TABLE books
-- DROP TABLE customers
-- DROP TABLE orders
-- DROP TABLE marketing_spend