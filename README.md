

# Supermarket Management System 

## Project Introduction & Objectives

The **Supermarket Management System** is a comprehensive C++ application designed to streamline inventory management, customer transactions, and administrative tasks for a retail environment. This system provides two main user roles—**administrators** and **buyers**—with distinct functionalities tailored to their needs.

### Key Objectives

- Provide administrators with tools for complete product inventory management  
- Enable customers to browse products, create wishlists, and place orders  
- Implement efficient data structures for fast product searches and updates  
- Generate business intelligence reports for decision making  
- Maintain data persistence through file storage  

---

## Data Structures and Algorithms

### Core Data Structures

- **Binary Search Tree (BST) for Products:**  
  - Products are stored in a BST indexed by product code  
  - Enables O(log n) average case for search, insert, and delete operations  
  - In-order traversal provides sorted output by product code  

- **Linked Lists:**  
  - Customer order history (singly linked list)  
  - Shopping cart items (singly linked list)  
  - Wishlist items (singly linked list)  
  - Customer records (singly linked list)  

- **Maps (STL):**  
  - Used for analytics to track category-wise product counts and revenue  
  - Sales reporting maps product codes to sales data pairs  

### Key Algorithms

- **BST Search Example:**
    ```cpp
    Product* Shopping::findProduct(Product* root, int code) {
        if (!root) return nullptr;
        if (root->code == code) return root;
        if (code < root->code) return findProduct(root->left, code);
        return findProduct(root->right, code);
    }
    ```

- **In-order Traversal with Filters:**
    ```cpp
    void Shopping::inOrderTraversal(Product* root, float minPrice, float maxPrice) {
        if (!root) return;
        inOrderTraversal(root->left, minPrice, maxPrice);
        if (root->price >= minPrice && root->price <= maxPrice) {
            cout << root->code << "\t" << root->name << "\t$" << root->price
                 << "\t" << root->discount << "%\t" << root->stock
                 << "\t" << root->category << "\n";
        }
        inOrderTraversal(root->right, minPrice, maxPrice);
    }
    ```

- **Sorting Algorithm:**
    ```cpp
    sort(products.begin(), products.end(),
        [](Product* a, Product* b){ return a->price < b->price; });
    ```

---

## UML Class Diagram (Textual)

```
+-------------------+
|     Shopping      |
+-------------------+
| - productRoot     |
| - customerHead    |
| - cartHead        |
| - currentCustomer |
+-------------------+
| + menu()          |
| + administrator() |
| + buyer()         |
| + adminLogin()    |
| + addProduct()    |
| + editProduct()   |
| ...               |
+-------------------+

+-------------------+
|     Product       |
+-------------------+
| - code            |
| - name            |
| - price           |
| - discount        |
| - stock           |
| - category        |
| - left            |
| - right           |
+-------------------+

+-------------------+
|    Customer       |
+-------------------+
| - username        |
| - password        |
| - loyaltyPoints   |
| - orderHistory    |
| - wishlist        |
| - next            |
+-------------------+

+-------------------+
|      Order        |
+-------------------+
| - code            |
| - productName     |
| - quantity        |
| - totalCost       |
| - next            |
+-------------------+
```

---

## Key Implementation Features

- **Product Management:**  
  - Add, edit, and delete products using BST for efficient operations.

- **Customer System:**  
  - Registration, login, and management of customer data using linked lists.

- **Order Processing:**  
  - Place orders, update stock, and maintain order history.

---

## Performance Analysis and Optimization

| Operation         | Data Structure | Average Case | Worst Case |
|-------------------|---------------|--------------|------------|
| Product Search    | BST           | O(log n)     | O(n)       |
| Product Insertion | BST           | O(log n)     | O(n)       |
| Product Deletion  | BST           | O(log n)     | O(n)       |
| Order History     | Linked List   | O(n)         | O(n)       |
| Category Analytics| Map           | O(1)         | O(1)       |

### Optimizations

- **BST Balancing:**  
  - Currently uses standard BST; future improvement: implement AVL or Red-Black tree for guaranteed O(log n).
- **Caching:**  
  - Frequently accessed products could be cached for faster access.
- **Batch Operations:**  
  - File I/O operations could be batched for better performance.

---

## Summary and Future Improvements

The Supermarket Management System successfully implements core retail management features using appropriate data structures. The BST provides efficient product management while linked lists handle customer-specific data. File persistence ensures data survives between sessions.

### Future Improvements

1. **Database Integration:** Replace file storage with SQL database for reliability and scalability  
2. **Enhanced Security:** Implement password hashing for customer accounts  
3. **Graphical Interface:** Develop Qt or web-based frontend for better user experience  
4. **Advanced Features:** Supplier management, employee scheduling, barcode scanning integration  
5. **Data Analysis:** Implement machine learning for sales prediction and customer behavior analysis  

---

## Installation

1. **Clone the repository:**
    ```bash
    git clone https://github.com/Qasimali-019/Shopping-management-system.git
    cd your-repo-name
    ```

2. **Compile the source code:**
    ```bash
    g++ -std=c++11 -o supermarket code.cpp
    ```

3. **Run the application:**
    ```bash
    ./supermarket
    ```

---

## Usage

- **Administrator:**  
  - Login with admin credentials  
  - Add, edit, or delete products  
  - View sales and inventory reports  

- **Buyer:**  
  - Register or login as a customer  
  - Browse products, add to cart or wishlist  
  - Place orders and view order history  

---

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository  
2. Create a new branch (`git checkout -b feature/your-feature`)  
3. Commit your changes (`git commit -am 'Add new feature'`)  
4. Push to the branch (`git push origin feature/your-feature`)  
5. Open a Pull Request  

---

## Contact

For questions or suggestions, please open an issue or contact qasim.aliii019@gmail.com

---
