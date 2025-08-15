#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <fstream>
#include <cctype>

using namespace std;

// ======================================
// Product Structure
// ======================================
struct Product
{
    int code;
    string name;
    float price;
    float discount;
    int stock;
    string category;
    Product *left;
    Product *right;
};

// ======================================
// Order Structure for Customers
// ======================================
struct Order
{
    int code;
    string productName;
    int quantity;
    float totalCost;
    Order *next;
};

// ======================================
// Customer Structure
// ======================================
struct Customer
{
    string username;
    string password;
    int loyaltyPoints;
    Order *orderHistory;
    Product *wishlist;
    Customer *next;
};

// ======================================
// Shopping Class
// ======================================
class Shopping
{
private:
    Product *productRoot;
    Customer *customerHead;
    Product *cartHead;
    Customer *currentCustomer;

public:
    Shopping() 
        : productRoot(nullptr), 
          customerHead(nullptr), 
          cartHead(nullptr), 
          currentCustomer(nullptr) 
    {}

    // ---------- Main menus ----------
    void menu();
    void administrator();
    void buyer();

    // ---------- Admin login ----------
    bool adminLogin();

    // ---------- Administrator functionalities ----------
    void addProduct();
    void editProduct();
    void deleteProduct();
    void listProducts();
    void listProductsByCategory();
    void lowStockAlert();
    void sortProductsByField(int field);
    void createPromotion();
    void viewAnalytics();

    // ---------- Buyer functionalities ----------
    void customerLogin();
    void customerRegistration();
    void placeOrder();
    void viewOrderHistory();
    void addToWishlist(int code);
    void viewWishlist();
    void addToCart(int code, int quantity);
    void modifyCart();
    void displayCart();

    // ---------- Search functionalities ----------
    void searchProductByName(string name);
    void searchProductByPriceRange(float minPrice, float maxPrice);

    // ---------- Reports/Utilities ----------
    void generateSalesReport();
    void saveAllProducts();
    void loadProductsOnStartup();

    // ---------- Internal utility functions ----------
private:
    // BST helpers
    Product *findProduct(Product *root, int code);
    Product *addProductToTree(Product *root, Product *newProduct);
    Product *deleteProductFromTree(Product *root, int code);
    Product *findMin(Product *root);

    void inOrderTraversal(Product *root);
    void inOrderTraversal(Product *root, const string &filter, const string &filterType);
    void inOrderTraversal(Product *root, float minPrice, float maxPrice);

    // Saving product data
    void saveProductsToFile(Product *root, ofstream &file);

    // Not strictly necessary here (used in your original code)
    void saveProductStockToFile(ofstream &productFile, Product *root) {}
};

// ========== ADMIN LOGIN ==========
bool Shopping::adminLogin()
{
    const string ADMIN_USERNAME = "admin";
    const string ADMIN_PASSWORD = "admin123";

    string username, password;
    cout << "Enter admin username: ";
    cin >> username;
    cout << "Enter admin password: ";
    cin >> password;

    if (username == ADMIN_USERNAME && password == ADMIN_PASSWORD)
    {
        cout << "Admin login successful!\n";
        return true;
    }
    else
    {
        cout << "Invalid admin credentials.\n";
        return false;
    }
}

// ========== FIND PRODUCT IN BST ==========
Product *Shopping::findProduct(Product *root, int code)
{
    // Standard BST search
    if (!root)
        return nullptr;

    if (root->code == code)
        return root;

    if (code < root->code)
        return findProduct(root->left, code);
    else
        return findProduct(root->right, code);
}

// ========== ADD PRODUCT TO BST ==========
Product *Shopping::addProductToTree(Product *root, Product *newProduct)
{
    if (!root)
        return newProduct;

    if (newProduct->code == root->code)
    {
        cout << "Error: Duplicate product code. Product not added.\n";
        return root;
    }

    if (newProduct->code < root->code)
        root->left = addProductToTree(root->left, newProduct);
    else
        root->right = addProductToTree(root->right, newProduct);

    return root;
}

// ========== FIND MIN FOR BST DELETION ==========
Product *Shopping::findMin(Product *root)
{
    while (root && root->left != nullptr)
        root = root->left;
    return root;
}

// ========== DELETE PRODUCT FROM BST ==========
Product *Shopping::deleteProductFromTree(Product *root, int code)
{
    if (!root)
    {
        cout << "Error: Product not found. Cannot delete.\n";
        return nullptr;
    }

    if (code < root->code)
    {
        root->left = deleteProductFromTree(root->left, code);
    }
    else if (code > root->code)
    {
        root->right = deleteProductFromTree(root->right, code);
    }
    else
    {
        // Product found
        // Node with zero or one child
        if (!root->left)
        {
            Product *temp = root->right;
            delete root;
            return temp;
        }
        if (!root->right)
        {
            Product *temp = root->left;
            delete root;
            return temp;
        }
        // Node with two children
        Product *temp = findMin(root->right);
        root->code = temp->code;
        root->name = temp->name;
        root->price = temp->price;
        root->discount = temp->discount;
        root->stock = temp->stock;
        root->category = temp->category;
        root->right = deleteProductFromTree(root->right, temp->code);
    }

    return root;
}

// ========== LOAD PRODUCTS ON STARTUP ==========
void Shopping::loadProductsOnStartup()
{
    ifstream file("products.txt");
    if (!file)
    {
        cout << "No product data file found. Starting with an empty inventory.\n";
        return;
    }

    while (!file.eof())
    {
        Product *newProduct = new Product;
        file >> newProduct->code 
             >> newProduct->name 
             >> newProduct->price 
             >> newProduct->discount 
             >> newProduct->stock 
             >> newProduct->category;

        if (file.fail())
        {
            // Prevent memory leaks if read fails
            delete newProduct;
            break;
        }

        newProduct->left = nullptr;
        newProduct->right = nullptr;

        productRoot = addProductToTree(productRoot, newProduct);
    }

    file.close();
    cout << "Products loaded successfully.\n";
}

// ========== SAVE ALL PRODUCTS ==========
void Shopping::saveAllProducts()
{
    ofstream file("products.txt");
    if (!file)
    {
        cout << "Error: Unable to save products to file.\n";
        return;
    }

    saveProductsToFile(productRoot, file);
    file.close();

    cout << "Product data saved successfully.\n";
}

// ========== RECURSIVE HELPER TO SAVE TO FILE ==========
void Shopping::saveProductsToFile(Product *root, ofstream &file)
{
    if (!root) return;
    saveProductsToFile(root->left, file);
    file << root->code << " " 
         << root->name << " " 
         << root->price << " " 
         << root->discount << " "
         << root->stock << " " 
         << root->category << "\n";
    saveProductsToFile(root->right, file);
}

// ========== ADMIN METHODS ==========

// -------------- ADD PRODUCT --------------
void Shopping::addProduct()
{
    Product *newProduct = new Product;

    cout << "Enter Product Code: ";
    cin >> newProduct->code;

    if (cin.fail() || newProduct->code <= 0)
    {
        cout << "Invalid Product Code! Code must be a positive integer.\n";
        cin.clear();
        cin.ignore(10000, '\n');
        delete newProduct;
        return;
    }

    // Check for duplicate
    if (findProduct(productRoot, newProduct->code) != nullptr)
    {
        cout << "Error: Product code already exists. Cannot add duplicate product.\n";
        delete newProduct;
        return;
    }

    // Product Name
    cout << "Enter Product Name: ";
    cin.ignore();
    getline(cin, newProduct->name);

    if (newProduct->name.empty())
    {
        cout << "Invalid Product Name! Name cannot be empty.\n";
        delete newProduct;
        return;
    }

    // Price
    cout << "Enter Product Price: ";
    cin >> newProduct->price;
    if (cin.fail() || newProduct->price <= 0)
    {
        cout << "Invalid Product Price! Price must be a positive number.\n";
        cin.clear();
        cin.ignore(10000, '\n');
        delete newProduct;
        return;
    }

    // Discount
    cout << "Enter Discount Percentage (0-100): ";
    cin >> newProduct->discount;
    if (cin.fail() || newProduct->discount < 0 || newProduct->discount > 100)
    {
        cout << "Invalid Discount! Must be between 0 and 100.\n";
        cin.clear();
        cin.ignore(10000, '\n');
        delete newProduct;
        return;
    }

    // Stock
    cout << "Enter Stock Quantity: ";
    cin >> newProduct->stock;
    if (cin.fail() || newProduct->stock < 0)
    {
        cout << "Invalid Stock Quantity! Stock cannot be negative.\n";
        cin.clear();
        cin.ignore(10000, '\n');
        delete newProduct;
        return;
    }

    // Category
    cout << "Enter Product Category: ";
    cin.ignore();
    getline(cin, newProduct->category);
    if (newProduct->category.empty())
    {
        cout << "Invalid Category! Category cannot be empty.\n";
        delete newProduct;
        return;
    }

    // Link pointers
    newProduct->left = nullptr;
    newProduct->right = nullptr;

    // Insert into BST
    productRoot = addProductToTree(productRoot, newProduct);

    cout << "Product added successfully!\n";
    cout << "---------------------------\n";
    cout << "Code: " << newProduct->code << "\n";
    cout << "Name: " << newProduct->name << "\n";
    cout << "Price: $" << newProduct->price << "\n";
    cout << "Discount: " << newProduct->discount << "%\n";
    cout << "Stock: " << newProduct->stock << "\n";
    cout << "Category: " << newProduct->category << "\n";

    // Log to file
    ofstream logFile("ProductLog.txt", ios::app);
    if (logFile.is_open())
    {
        logFile << "Product Added:\n";
        logFile << "Code: " << newProduct->code << "\n";
        logFile << "Name: " << newProduct->name << "\n";
        logFile << "Price: $" << newProduct->price << "\n";
        logFile << "Discount: " << newProduct->discount << "%\n";
        logFile << "Stock: " << newProduct->stock << "\n";
        logFile << "Category: " << newProduct->category << "\n";
        logFile << "---------------------------------------\n";
        logFile.close();
    }
    else
    {
        cout << "Error: Unable to open log file for writing.\n";
    }
}

// -------------- EDIT PRODUCT --------------
void Shopping::editProduct()
{
    int code;
    cout << "Enter the Product Code to edit: ";
    cin >> code;

    Product *product = findProduct(productRoot, code);
    if (!product)
    {
        cout << "Product not found.\n";
        return;
    }

    cout << "\nEditing Product: " << product->name << "\n";
    cout << "---------------------------------------\n";
    cout << "Existing Details:\n";
    cout << "Code: " << product->code << "\n";
    cout << "Name: " << product->name << "\n";
    cout << "Price: $" << product->price << "\n";
    cout << "Discount: " << product->discount << "%\n";
    cout << "Stock: " << product->stock << "\n";
    cout << "Category: " << product->category << "\n";
    cout << "---------------------------------------\n";

    cout << "Enter New Name (leave empty to keep existing): ";
    cin.ignore();
    string newName;
    getline(cin, newName);
    if (!newName.empty())
        product->name = newName;

    cout << "Enter New Price (-1 to keep existing): ";
    float newPrice;
    cin >> newPrice;
    if (newPrice >= 0)
        product->price = newPrice;

    cout << "Enter New Discount Percentage (0-100, -1 to keep existing): ";
    float newDiscount;
    cin >> newDiscount;
    if (newDiscount >= 0 && newDiscount <= 100)
        product->discount = newDiscount;

    cout << "Enter New Stock Quantity (-1 to keep existing): ";
    int newStock;
    cin >> newStock;
    if (newStock >= 0)
        product->stock = newStock;

    cout << "Enter New Category (leave empty to keep existing): ";
    cin.ignore();
    string newCategory;
    getline(cin, newCategory);
    if (!newCategory.empty())
        product->category = newCategory;

    cout << "Product updated successfully!\n";
    cout << "---------------------------------------\n";
    cout << "Updated Details:\n";
    cout << "Code: " << product->code << "\n";
    cout << "Name: " << product->name << "\n";
    cout << "Price: $" << product->price << "\n";
    cout << "Discount: " << product->discount << "%\n";
    cout << "Stock: " << product->stock << "\n";
    cout << "Category: " << product->category << "\n";
    cout << "---------------------------------------\n";

    // Log
    ofstream logFile("ProductLog.txt", ios::app);
    if (logFile.is_open())
    {
        logFile << "Product Edited:\n";
        logFile << "Code: " << product->code << "\n";
        logFile << "Name: " << product->name << "\n";
        logFile << "Price: $" << product->price << "\n";
        logFile << "Discount: " << product->discount << "%\n";
        logFile << "Stock: " << product->stock << "\n";
        logFile << "Category: " << product->category << "\n";
        logFile << "---------------------------------------\n";
        logFile.close();
    }
    else
    {
        cout << "Error: Unable to open log file for writing.\n";
    }
}

// -------------- DELETE PRODUCT --------------
void Shopping::deleteProduct()
{
    int code;
    cout << "Enter the Product Code to delete: ";
    cin >> code;

    // First check
    Product *product = findProduct(productRoot, code);
    if (!product)
    {
        cout << "Product not found.\n";
        return;
    }

    cout << "\nProduct Details:\n";
    cout << "---------------------------------------\n";
    cout << "Code: " << product->code << "\n";
    cout << "Name: " << product->name << "\n";
    cout << "Price: $" << product->price << "\n";
    cout << "Discount: " << product->discount << "%\n";
    cout << "Stock: " << product->stock << "\n";
    cout << "Category: " << product->category << "\n";
    cout << "---------------------------------------\n";

    char confirm;
    cout << "Are you sure you want to delete this product? (Y/N): ";
    cin >> confirm;
    if (tolower(confirm) != 'y')
    {
        cout << "Product deletion canceled.\n";
        return;
    }

    productRoot = deleteProductFromTree(productRoot, code);
    cout << "Product deleted successfully!\n";

    // Log
    ofstream logFile("ProductLog.txt", ios::app);
    if (logFile.is_open())
    {
        logFile << "Product Deleted:\n";
        logFile << "Code: " << code << "\n";
        logFile << "---------------------------------------\n";
        logFile.close();
    }
    else
    {
        cout << "Error: Unable to open log file for writing.\n";
    }
}

// -------------- LIST ALL PRODUCTS --------------
void Shopping::listProducts()
{
    if (!productRoot)
    {
        cout << "No products found in memory. Reloading from file...\n";
        ifstream file("products.txt");
        if (!file)
        {
            cout << "Error: No product data file found.\n";
            return;
        }

        while (!file.eof())
        {
            Product *newProduct = new Product;
            file >> newProduct->code 
                 >> newProduct->name 
                 >> newProduct->price 
                 >> newProduct->discount 
                 >> newProduct->stock 
                 >> newProduct->category;

            if (file.fail())
            {
                delete newProduct;
                break;
            }

            newProduct->left = newProduct->right = nullptr;
            productRoot = addProductToTree(productRoot, newProduct);
        }
        file.close();
    }

    if (!productRoot)
    {
        cout << "No products available to list.\n";
        return;
    }

    cout << "===================================================================\n";
    cout << "Code\tName\t\tPrice\tDiscount\tStock\tCategory\n";
    cout << "===================================================================\n";
    inOrderTraversal(productRoot);
    cout << "===================================================================\n";
}

// -------------- LIST PRODUCTS BY CATEGORY --------------
void Shopping::listProductsByCategory()
{
    if (!productRoot)
    {
        cout << "No products available.\n";
        return;
    }

    cout << "Enter the category to list products: ";
    cin.ignore();
    string category;
    getline(cin, category);

    cout << "\nProducts in Category: " << category << "\n";
    cout << "===================================================================\n";
    cout << "Code\tName\t\tPrice\tDiscount\tStock\n";
    cout << "===================================================================\n";

    // Lambda to traverse & filter
    function<void(Product*)> inOrderTraversalByCategory = [&](Product *root)
    {
        if (!root) return;
        inOrderTraversalByCategory(root->left);
        if (root->category == category)
        {
            cout << root->code << "\t" << root->name << "\t\t$" << root->price
                 << "\t" << root->discount << "%\t\t" << root->stock << "\n";
        }
        inOrderTraversalByCategory(root->right);
    };

    inOrderTraversalByCategory(productRoot);

    cout << "===================================================================\n";

    // Log
    ofstream logFile("ProductLog.txt", ios::app);
    if (logFile.is_open())
    {
        logFile << "Listed Products by Category: " << category << "\n";
        logFile << "---------------------------------------\n";
        logFile.close();
    }
    else
    {
        cout << "Error: Unable to open log file for writing.\n";
    }
}

// -------------- LOW STOCK ALERT --------------
void Shopping::lowStockAlert()
{
    if (!productRoot)
    {
        cout << "No products available.\n";
        return;
    }

    int threshold;
    cout << "Enter the stock threshold for alert: ";
    cin >> threshold;

    cout << "\nLow Stock Products (Stock < " << threshold << "):\n";
    cout << "===================================================================\n";
    cout << "Code\tName\t\tPrice\tDiscount\tStock\tCategory\n";
    cout << "===================================================================\n";

    function<void(Product*)> inOrderTraversalLowStock = [&](Product *root)
    {
        if (!root) return;
        inOrderTraversalLowStock(root->left);
        if (root->stock < threshold)
        {
            cout << root->code << "\t" << root->name << "\t\t$" << root->price
                 << "\t" << root->discount << "%\t\t" << root->stock
                 << "\t" << root->category << "\n";
        }
        inOrderTraversalLowStock(root->right);
    };

    inOrderTraversalLowStock(productRoot);

    cout << "===================================================================\n";

    // Log
    ofstream logFile("ProductLog.txt", ios::app);
    if (logFile.is_open())
    {
        logFile << "Generated Low Stock Alert (Threshold: " << threshold << "):\n";
        logFile << "---------------------------------------\n";
        logFile.close();
    }
    else
    {
        cout << "Error: Unable to open log file for writing.\n";
    }
}

// -------------- SORT PRODUCTS BY FIELD --------------
void Shopping::sortProductsByField(int field)
{
    if (!productRoot)
    {
        cout << "No products available to sort.\n";
        return;
    }

    // Collect all products into a vector
    vector<Product*> products;
    function<void(Product*)> collectProducts = [&](Product *root)
    {
        if (!root) return;
        collectProducts(root->left);
        products.push_back(root);
        collectProducts(root->right);
    };
    collectProducts(productRoot);

    // Sort based on field
    switch (field)
    {
    case 1: // Name
        sort(products.begin(), products.end(), 
            [](Product *a, Product *b){ return a->name < b->name; });
        break;
    case 2: // Price
        sort(products.begin(), products.end(), 
            [](Product *a, Product *b){ return a->price < b->price; });
        break;
    case 3: // Stock
        sort(products.begin(), products.end(), 
            [](Product *a, Product *b){ return a->stock < b->stock; });
        break;
    default:
        cout << "Invalid sorting field. Please choose 1 (Name), 2 (Price), or 3 (Stock).\n";
        return;
    }

    // Display sorted
    cout << "\nSorted Products:\n";
    cout << "===================================================================\n";
    cout << "Code\tName\t\tPrice\tDiscount\tStock\tCategory\n";
    cout << "===================================================================\n";

    for (auto product : products)
    {
        cout << product->code << "\t" << product->name << "\t\t$" << product->price
             << "\t" << product->discount << "%\t\t" << product->stock
             << "\t" << product->category << "\n";
    }
    cout << "===================================================================\n";

    // Log
    ofstream logFile("ProductLog.txt", ios::app);
    if (logFile.is_open())
    {
        logFile << "Sorted Products by Field (" << field << "):\n";
        for (auto product : products)
        {
            logFile << "Code: " << product->code
                    << ", Name: " << product->name
                    << ", Price: $" << product->price
                    << ", Stock: " << product->stock
                    << ", Category: " << product->category << "\n";
        }
        logFile << "---------------------------------------\n";
        logFile.close();
    }
    else
    {
        cout << "Error: Unable to open log file for writing.\n";
    }
}

// -------------- CREATE PROMOTION --------------
void Shopping::createPromotion()
{
    int promotionType;
    cout << "Create Promotion:\n";
    cout << "1) Discount on a Specific Product\n";
    cout << "2) Discount on a Category\n";
    cout << "3) General Discount on All Products\n";
    cout << "Enter your choice: ";
    cin >> promotionType;

    if (promotionType < 1 || promotionType > 3)
    {
        cout << "Invalid choice. Please select 1, 2, or 3.\n";
        return;
    }

    ofstream logFile("PromotionLog.txt", ios::app);
    if (!logFile.is_open())
    {
        cout << "Error: Unable to open promotion log file.\n";
        return;
    }

    logFile << "New Promotion Created:\n";

    switch (promotionType)
    {
    case 1:
    {
        // Specific Product
        int code;
        float discount;
        cout << "Enter Product Code: ";
        cin >> code;
        cout << "Enter Discount Percentage (0-100): ";
        cin >> discount;

        if (discount < 0 || discount > 100)
        {
            cout << "Invalid discount percentage. Must be between 0 and 100.\n";
            logFile.close();
            return;
        }

        Product *product = findProduct(productRoot, code);
        if (!product)
        {
            cout << "Product not found.\n";
            logFile.close();
            return;
        }

        product->discount = discount;
        cout << "Discount of " << discount << "% applied to product: " << product->name << "\n";
        logFile << "Promotion Type: Specific Product\n";
        logFile << "Product Code: " << product->code << ", Name: " << product->name
                << ", Discount: " << discount << "%\n";
        break;
    }
    case 2:
    {
        // Category
        string category;
        float discount;
        cout << "Enter Category: ";
        cin.ignore();
        getline(cin, category);
        cout << "Enter Discount Percentage (0-100): ";
        cin >> discount;
        if (discount < 0 || discount > 100)
        {
            cout << "Invalid discount percentage. Must be between 0 and 100.\n";
            logFile.close();
            return;
        }

        function<void(Product*)> applyCategoryDiscount = [&](Product *root)
        {
            if (!root) return;
            applyCategoryDiscount(root->left);
            if (root->category == category)
            {
                root->discount = discount;
                cout << "Discount of " << discount << "% applied to product: " << root->name << "\n";
            }
            applyCategoryDiscount(root->right);
        };
        applyCategoryDiscount(productRoot);

        logFile << "Promotion Type: Category Discount\n";
        logFile << "Category: " << category << ", Discount: " << discount << "%\n";
        break;
    }
    case 3:
    {
        // General discount
        float discount;
        cout << "Enter Discount Percentage (0-100): ";
        cin >> discount;
        if (discount < 0 || discount > 100)
        {
            cout << "Invalid discount percentage. Must be between 0 and 100.\n";
            logFile.close();
            return;
        }

        function<void(Product*)> applyGeneralDiscount = [&](Product *root)
        {
            if (!root) return;
            applyGeneralDiscount(root->left);
            root->discount = discount;
            cout << "Discount of " << discount << "% applied to product: " << root->name << "\n";
            applyGeneralDiscount(root->right);
        };
        applyGeneralDiscount(productRoot);

        logFile << "Promotion Type: General Discount\n";
        logFile << "Discount: " << discount << "%\n";
        break;
    }
    }

    logFile << "---------------------------------------\n";
    logFile.close();
    cout << "Promotion created successfully!\n";
}

// -------------- VIEW ANALYTICS --------------
void Shopping::viewAnalytics()
{
    if (!productRoot)
    {
        cout << "No products available to analyze.\n";
        return;
    }

    int totalProducts = 0;
    int lowStockCount = 0;
    float totalRevenue = 0.0;

    map<string, int> categoryCounts;
    map<string, float> categoryRevenue;

    Product *mostPopularProduct = nullptr;
    int highestSales = 0;

    function<void(Product*)> calculateAnalytics = [&](Product *root)
    {
        if (!root) return;
        calculateAnalytics(root->left);

        totalProducts++;
        if (root->stock < 10)
            lowStockCount++;

        float productRevenue = (root->price * root->stock) * (1 - root->discount / 100.0f);
        totalRevenue += productRevenue;

        categoryCounts[root->category]++;
        categoryRevenue[root->category] += productRevenue;

        if (root->stock > highestSales)
        {
            highestSales = root->stock;
            mostPopularProduct = root;
        }

        calculateAnalytics(root->right);
    };
    calculateAnalytics(productRoot);

    cout << "\nAnalytics Dashboard\n";
    cout << "========================================================\n";
    cout << "Total Products in Inventory: " << totalProducts << "\n";
    cout << "Total Revenue (Estimate): $" << totalRevenue << "\n";
    cout << "Low Stock Products (Stock < 10): " << lowStockCount << "\n";
    cout << "Most Popular Product: " 
         << (mostPopularProduct ? mostPopularProduct->name : "N/A") << "\n";
    cout << "========================================================\n";

    cout << "\nCategory-wise Product Counts:\n";
    for (auto &cat : categoryCounts)
        cout << "Category: " << cat.first << " - Products: " << cat.second << "\n";

    cout << "\nCategory-wise Revenue:\n";
    for (auto &cat : categoryRevenue)
        cout << "Category: " << cat.first << " - Revenue: $" << cat.second << "\n";

    cout << "========================================================\n";

    // Log analytics
    ofstream logFile("AnalyticsLog.txt", ios::app);
    if (logFile.is_open())
    {
        logFile << "Analytics Report:\n";
        logFile << "Total Products: " << totalProducts << "\n";
        logFile << "Total Revenue: $" << totalRevenue << "\n";
        logFile << "Low Stock Products: " << lowStockCount << "\n";
        if (mostPopularProduct)
        {
            logFile << "Most Popular Product: " << mostPopularProduct->name
                    << " (Stock: " << mostPopularProduct->stock << ")\n";
        }
        logFile << "\nCategory-wise Product Counts:\n";
        for (auto &cat : categoryCounts)
            logFile << "Category: " << cat.first << " - Products: " << cat.second << "\n";

        logFile << "\nCategory-wise Revenue:\n";
        for (auto &cat : categoryRevenue)
            logFile << "Category: " << cat.first << " - Revenue: $" << cat.second << "\n";

        logFile << "---------------------------------------\n";
        logFile.close();
    }
    else
    {
        cout << "Error: Unable to open analytics log file for writing.\n";
    }
}

// ========== BUYER METHODS ==========

// -------------- REGISTER CUSTOMER --------------
void Shopping::customerRegistration()
{
    string username, password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    // Optional check: if user file already exists
    ifstream checkFile(username + ".txt");
    if (checkFile.good())
    {
        cout << "User already exists. Please try logging in.\n";
        return;
    }
    checkFile.close();

    ofstream customerFile(username + ".txt");
    if (!customerFile)
    {
        cout << "Error creating customer file.\n";
        return;
    }

    customerFile << username << "\n" << password << "\n";
    customerFile.close();

    // Create new Customer in memory
    Customer *newCustomer = new Customer{username, password, 0, nullptr, nullptr, nullptr};
    newCustomer->next = customerHead;
    customerHead = newCustomer;

    // Set as current
    currentCustomer = newCustomer;

    cout << "Registration successful. Welcome, " << username << "!\n";
}

// -------------- LOGIN CUSTOMER --------------
void Shopping::customerLogin()
{
    string username, password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    ifstream customerFile(username + ".txt");
    if (!customerFile)
    {
        cout << "Invalid username or password.\n";
        return;
    }

    string storedUsername, storedPassword;
    customerFile >> storedUsername >> storedPassword;
    customerFile.close();

    if (storedUsername == username && storedPassword == password)
    {
        // Create or find this user in the linked list
        // For simplicity, we'll just create a new node each login
        Customer *newCustomer = new Customer{username, password, 0, nullptr, nullptr, nullptr};
        newCustomer->next = customerHead;
        customerHead = newCustomer;

        currentCustomer = newCustomer;

        cout << "Login successful. Welcome, " << username << "!\n";
    }
    else
    {
        cout << "Invalid username or password.\n";
    }
}

// -------------- PLACE ORDER --------------
void Shopping::placeOrder()
{
    if (!currentCustomer)
    {
        cout << "Please log in to place an order.\n";
        return;
    }
    if (!cartHead)
    {
        cout << "Your cart is empty.\n";
        return;
    }

    float totalCost = 0.0f;
    ofstream orderFile(currentCustomer->username + "_orders.txt", ios::app);
    if (!orderFile)
    {
        cout << "Error: Unable to save order history.\n";
        return;
    }

    cout << "\nFinalizing Order:\n";
    cout << "===================================================================\n";
    cout << "Code\tName\t\tPrice\tDiscount\tQuantity\tTotal\n";
    cout << "===================================================================\n";

    Product *temp = cartHead;
    while (temp)
    {
        float itemCost = temp->price * temp->stock * (1 - temp->discount / 100.0f);
        totalCost += itemCost;

        // Deduct from main inventory
        Product *product = findProduct(productRoot, temp->code);
        if (product)
        {
            product->stock -= temp->stock;
        }

        // Write to order file (for permanent record)
        orderFile << temp->code << " " << temp->name << " " << temp->stock << " " << itemCost << "\n";

        // Also store the order in the currentCustomer->orderHistory
        Order *newOrder = new Order{temp->code, temp->name, temp->stock, itemCost, nullptr};
        if (!currentCustomer->orderHistory)
        {
            currentCustomer->orderHistory = newOrder;
        }
        else
        {
            // append to the end
            Order *ordTemp = currentCustomer->orderHistory;
            while (ordTemp->next) ordTemp = ordTemp->next;
            ordTemp->next = newOrder;
        }

        // Display
        cout << temp->code << "\t" << temp->name << "\t\t$" << temp->price 
             << "\t" << temp->discount << "%\t" << temp->stock 
             << "\t$" << itemCost << "\n";

        // Move to next cart item
        Product *toDelete = temp;
        temp = temp->left;  // 'left' is acting like 'next' in cart
        delete toDelete;
    }
    cartHead = nullptr; // cart is now empty
    orderFile.close();

    cout << "===================================================================\n";
    cout << "Total Cost: $" << totalCost << "\n";
    cout << "===================================================================\n";
}

// -------------- VIEW ORDER HISTORY --------------
void Shopping::viewOrderHistory()
{
    if (!currentCustomer)
    {
        cout << "Please log in first.\n";
        return;
    }

    string fileName = currentCustomer->username + "_orders.txt";
    ifstream orderFile(fileName);
    if (!orderFile)
    {
        cout << "No order history found for " << currentCustomer->username << ".\n";
        return;
    }

    cout << "\nOrder History for " << currentCustomer->username << ":\n";
    cout << "===================================================================\n";
    cout << "Product Code\tProduct Name\tQuantity\tTotal Cost\n";
    cout << "===================================================================\n";

    int code, quantity;
    string productName;
    float totalCost;
    bool hasOrders = false;

    while (orderFile >> code >> productName >> quantity >> totalCost)
    {
        if (orderFile.fail())
        {
            cout << "Error reading order history. Please check the file format.\n";
            break;
        }
        cout << code << "\t\t" << productName << "\t" << quantity << "\t\t$" << totalCost << "\n";
        hasOrders = true;
    }

    if (!hasOrders)
    {
        cout << "No orders found for " << currentCustomer->username << ".\n";
    }
    cout << "===================================================================\n";

    orderFile.close();
}

// -------------- ADD TO WISHLIST --------------
void Shopping::addToWishlist(int code)
{
    if (!currentCustomer)
    {
        cout << "Please log in to add items to your wishlist.\n";
        return;
    }

    Product *product = findProduct(productRoot, code);
    if (!product)
    {
        cout << "Product not found.\n";
        return;
    }

    // Add a copy of the product to the wishlist linked list
    Product *newWishlistItem = new Product{product->code, product->name, product->price, product->discount, product->stock, product->category, nullptr, nullptr};
    newWishlistItem->left = currentCustomer->wishlist;
    currentCustomer->wishlist = newWishlistItem;

    // Also append to wishlist file
    ofstream wishlistFile(currentCustomer->username + "_wishlist.txt", ios::app);
    wishlistFile << product->code << " " << product->name << " " << product->price << "\n";
    wishlistFile.close();

    cout << "Product " << product->name << " added to wishlist.\n";
}

// -------------- VIEW WISHLIST --------------
void Shopping::viewWishlist()
{
    if (!currentCustomer)
    {
        cout << "Please log in first.\n";
        return;
    }
    ifstream wishlistFile(currentCustomer->username + "_wishlist.txt");
    if (!wishlistFile)
    {
        cout << "Your wishlist is empty.\n";
        return;
    }

    cout << "\nWishlist for " << currentCustomer->username << ":\n";
    cout << "===================================================================\n";
    cout << "Product Code\tProduct Name\tPrice\n";
    cout << "===================================================================\n";

    int code;
    string name;
    float price;
    while (wishlistFile >> code >> name >> price)
    {
        cout << code << "\t\t" << name << "\t\t$" << price << "\n";
    }
    cout << "===================================================================\n";

    wishlistFile.close();
}

// -------------- ADD TO CART --------------
void Shopping::addToCart(int code, int quantity)
{
    if (quantity <= 0)
    {
        cout << "Error: Quantity must be greater than 0.\n";
        return;
    }

    Product *product = findProduct(productRoot, code);
    if (!product)
    {
        cout << "Product not found.\n";
        return;
    }

    if (quantity > product->stock)
    {
        cout << "Error: Not enough stock available. Stock remaining: " << product->stock << "\n";
        return;
    }

    // Reduce stock from main inventory right away
    product->stock -= quantity;

    // If already in cart, update quantity
    Product *temp = cartHead;
    while (temp)
    {
        if (temp->code == code)
        {
            temp->stock += quantity; 
            cout << "Updated quantity of " << temp->name << " in cart to " << temp->stock << ".\n";
            return;
        }
        temp = temp->left;
    }

    // Otherwise, add a new node to cart
    Product *cartItem = new Product{product->code, product->name, product->price, product->discount, quantity, product->category, nullptr, nullptr};
    cartItem->left = cartHead;
    cartHead = cartItem;

    cout << "Added " << quantity << " units of " << product->name << " to the cart.\n";
}

// -------------- MODIFY CART --------------
void Shopping::modifyCart()
{
    if (!cartHead)
    {
        cout << "Your cart is empty.\n";
        return;
    }

    int code, quantity;
    cout << "Enter Product Code to modify: ";
    cin >> code;

    Product *cartItem = cartHead;
    Product *prev = nullptr;
    while (cartItem && cartItem->code != code)
    {
        prev = cartItem;
        cartItem = cartItem->left;
    }

    if (!cartItem)
    {
        cout << "Product not found in the cart.\n";
        return;
    }

    cout << "Enter New Quantity (0 to remove): ";
    cin >> quantity;
    if (quantity == 0)
    {
        // remove
        if (!prev)
            cartHead = cartItem->left;
        else
            prev->left = cartItem->left;

        cout << "Removed " << cartItem->name << " from the cart.\n";
        delete cartItem;
    }
    else
    {
        cartItem->stock = quantity;
        cout << "Updated quantity of " << cartItem->name << " to " << quantity << ".\n";
    }
}

// -------------- DISPLAY CART --------------
void Shopping::displayCart()
{
    if (!cartHead)
    {
        cout << "Your cart is empty.\n";
        return;
    }

    cout << "Cart Items:\n";
    cout << "===================================================================\n";
    cout << "Code\tName\t\tPrice\tDiscount\tQuantity\n";
    cout << "===================================================================\n";

    Product *temp = cartHead;
    while (temp)
    {
        cout << temp->code << "\t" << temp->name << "\t\t$" << temp->price 
             << "\t" << temp->discount << "%\t" << temp->stock << "\n";
        temp = temp->left;
    }
    cout << "===================================================================\n";
}

// -------------- SEARCH BY NAME --------------
void Shopping::searchProductByName(string name)
{
    // to lowercase
    transform(name.begin(), name.end(), name.begin(), ::tolower);
    bool found = false;

    cout << "Products matching the name '" << name << "':\n";
    cout << "===================================================================\n";
    cout << "Code\tName\t\tPrice\tDiscount\tStock\tCategory\n";
    cout << "===================================================================\n";

    function<void(Product*)> searchByName = [&](Product *root)
    {
        if (!root) return;
        searchByName(root->left);

        // Lowercase product name for matching
        string productName = root->name;
        transform(productName.begin(), productName.end(), productName.begin(), ::tolower);

        if (productName.find(name) != string::npos)
        {
            found = true;
            cout << root->code << "\t" << root->name << "\t\t$" << root->price 
                 << "\t" << root->discount << "%\t" << root->stock 
                 << "\t" << root->category << "\n";
        }
        searchByName(root->right);
    };
    searchByName(productRoot);

    if (!found)
        cout << "No products found matching '" << name << "'.\n";

    cout << "===================================================================\n";
}

// -------------- SEARCH BY PRICE RANGE --------------
void Shopping::searchProductByPriceRange(float minPrice, float maxPrice)
{
    cout << "Products in the price range $" << minPrice << " - $" << maxPrice << ":\n";
    cout << "===================================================================\n";
    cout << "Code\tName\t\tPrice\tDiscount\tStock\tCategory\n";
    cout << "===================================================================\n";

    inOrderTraversal(productRoot, minPrice, maxPrice);

    cout << "===================================================================\n";
}

// ========== IN-ORDER TRAVERSALS ==========

// -------------- HELPER: IN-ORDER (NO FILTER) --------------
void Shopping::inOrderTraversal(Product *root)
{
    if (!root) return;
    inOrderTraversal(root->left);
    cout << root->code << "\t" << root->name << "\t$" << root->price 
         << "\t" << root->discount << "%\t" << root->stock 
         << "\t" << root->category << "\n";
    inOrderTraversal(root->right);
}

// -------------- HELPER: IN-ORDER (FILTER: category) --------------
void Shopping::inOrderTraversal(Product *root, const string &filter, const string &filterType)
{
    if (!root) return;
    inOrderTraversal(root->left, filter, filterType);

    if (filterType == "category" && root->category == filter)
    {
        cout << root->code << "\t" << root->name << "\t$" << root->price 
             << "\t" << root->discount << "%\t" << root->stock 
             << "\t" << root->category << "\n";
    }

    inOrderTraversal(root->right, filter, filterType);
}

// -------------- HELPER: IN-ORDER (FILTER: price range) --------------
void Shopping::inOrderTraversal(Product *root, float minPrice, float maxPrice)
{
    if (!root) return;
    inOrderTraversal(root->left, minPrice, maxPrice);

    if (root->price >= minPrice && root->price <= maxPrice)
    {
        cout << root->code << "\t" << root->name << "\t$" << root->price 
             << "\t" << root->discount << "%\t" << root->stock 
             << "\t" << root->category << "\n";
    }

    inOrderTraversal(root->right, minPrice, maxPrice);
}

// -------------- GENERATE SALES REPORT --------------
void Shopping::generateSalesReport()
{
    if (!customerHead)
    {
        cout << "No customers found. Sales data unavailable.\n";
        return;
    }

    // Map: productCode -> (totalQtySold, totalRevenue)
    map<int, pair<int, float>> salesData;

    // Traverse all customers
    Customer *cptr = customerHead;
    while (cptr)
    {
        Order *order = cptr->orderHistory;
        while (order)
        {
            salesData[order->code].first  += order->quantity;
            salesData[order->code].second += order->totalCost;
            order = order->next;
        }
        cptr = cptr->next;
    }

    if (salesData.empty())
    {
        cout << "No sales data available.\n";
        return;
    }

    cout << "\nSales Report:\n";
    cout << "===================================================================\n";
    cout << "Code\tName\t\tTotal Quantity\tTotal Revenue\n";
    cout << "===================================================================\n";

    for (auto &entry : salesData)
    {
        int productCode = entry.first;
        int totalQuantity = entry.second.first;
        float totalRevenue = entry.second.second;

        Product *product = findProduct(productRoot, productCode);
        if (product)
        {
            cout << product->code << "\t" << product->name << "\t\t" 
                 << totalQuantity << "\t\t$" << totalRevenue << "\n";
        }
        else
        {
            cout << productCode << "\t" << "Unknown Product" << "\t\t" 
                 << totalQuantity << "\t\t$" << totalRevenue << "\n";
        }
    }

    cout << "===================================================================\n";

    // Save to file
    ofstream reportFile("SalesReport.txt", ios::app);
    if (reportFile.is_open())
    {
        reportFile << "Sales Report:\n";
        reportFile << "===================================================================\n";
        reportFile << "Code\tName\t\tTotal Quantity\tTotal Revenue\n";
        reportFile << "===================================================================\n";

        for (auto &entry : salesData)
        {
            int productCode = entry.first;
            int totalQuantity = entry.second.first;
            float totalRev = entry.second.second;

            Product *product = findProduct(productRoot, productCode);
            if (product)
            {
                reportFile << product->code << "\t" << product->name << "\t\t" 
                           << totalQuantity << "\t\t$" << totalRev << "\n";
            }
            else
            {
                reportFile << productCode << "\t" << "Unknown Product" << "\t\t"
                           << totalQuantity << "\t\t$" << totalRev << "\n";
            }
        }

        reportFile << "===================================================================\n";
        reportFile.close();
    }
    else
    {
        cout << "Error: Unable to write sales report to file.\n";
    }
}

// ========== MAIN MENUS ==========

// -------------- MAIN MENU --------------
void Shopping::menu()
{
    int choice;
    do
    {
        cout << "\n\t\tWelcome to the Supermarket System\n";
        cout << "=====================================\n";
        cout << "1) Administrator\n";
        cout << "2) Buyer\n";
        cout << "3) Exit\n";
        cout << "=====================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            // Force admin to login first
            bool loggedIn = adminLogin();
            if (loggedIn)
            {
                administrator();
            }
            break;
        }
        case 2:
            buyer();
            break;
        case 3:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 3);
}

// -------------- ADMINISTRATOR MENU --------------
void Shopping::administrator()
{
    int choice;
    do
    {
        cout << "\nAdministrator Menu\n";
        cout << "=========================\n";
        cout << "1) Add Product\n";
        cout << "2) Edit Product\n";
        cout << "3) Delete Product\n";
        cout << "4) List Products\n";
        cout << "5) List Products by Category\n";
        cout << "6) Search Products by Name\n";
        cout << "7) Search Products by Price Range\n";
        cout << "8) Generate Sales Report\n";
        cout << "9) Low Stock Alerts\n";
        cout << "10) Sort Products by Field\n";
        cout << "11) Create Promotions\n";
        cout << "12) View Analytics\n";
        cout << "13) Back to Main Menu\n";
        cout << "=========================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            addProduct();
            break;
        case 2:
            editProduct();
            break;
        case 3:
            deleteProduct();
            break;
        case 4:
            listProducts();
            break;
        case 5:
            listProductsByCategory();
            break;
        case 6:
        {
            string name;
            cout << "Enter Product Name: ";
            cin >> name;
            searchProductByName(name);
            break;
        }
        case 7:
        {
            float minPrice, maxPrice;
            cout << "Enter Min Price: ";
            cin >> minPrice;
            cout << "Enter Max Price: ";
            cin >> maxPrice;
            searchProductByPriceRange(minPrice, maxPrice);
            break;
        }
        case 8:
            generateSalesReport();
            break;
        case 9:
            lowStockAlert();
            break;
        case 10:
        {
            int field;
            cout << "Sort by: 1) Name 2) Price 3) Stock\n";
            cin >> field;
            sortProductsByField(field);
            break;
        }
        case 11:
            createPromotion();
            break;
        case 12:
            viewAnalytics();
            break;
        case 13:
            return; // back to main menu
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (true);
}

// -------------- BUYER MENU --------------
void Shopping::buyer()
{
    int choice;
    do
    {
        cout << "\nBuyer Menu\n";
        cout << "=========================\n";
        cout << "1) Login\n";
        cout << "2) Register\n";
        cout << "3) Add to Cart\n";
        cout << "4) Modify Cart\n";
        cout << "5) Display Cart\n";
        cout << "6) Place Order\n";
        cout << "7) View Order History\n";
        cout << "8) Add to Wishlist\n";
        cout << "9) View Wishlist\n";
        cout << "10) Logout\n";
        cout << "11) Back to Main Menu\n";
        cout << "=========================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            customerLogin();
            break;
        case 2:
            customerRegistration();
            break;
        case 3:
        {
            if (!currentCustomer)
            {
                cout << "Please log in to add items to your cart.\n";
                break;
            }
            int code, quantity;
            cout << "Enter Product Code: ";
            cin >> code;
            cout << "Enter Quantity: ";
            cin >> quantity;
            addToCart(code, quantity);
            break;
        }
        case 4:
        {
            if (!currentCustomer)
            {
                cout << "Please log in to modify your cart.\n";
                break;
            }
            modifyCart();
            break;
        }
        case 5:
        {
            if (!currentCustomer)
            {
                cout << "Please log in to display your cart.\n";
                break;
            }
            displayCart();
            break;
        }
        case 6:
        {
            if (!currentCustomer)
            {
                cout << "Please log in to place an order.\n";
                break;
            }
            placeOrder();
            break;
        }
        case 7:
        {
            if (!currentCustomer)
            {
                cout << "Please log in to view your order history.\n";
                break;
            }
            viewOrderHistory();
            break;
        }
        case 8:
        {
            if (!currentCustomer)
            {
                cout << "Please log in to add items to wishlist.\n";
                break;
            }
            int code;
            cout << "Enter Product Code to Add to Wishlist: ";
            cin >> code;
            addToWishlist(code);
            break;
        }
        case 9:
        {
            if (!currentCustomer)
            {
                cout << "Please log in first.\n";
                break;
            }
            viewWishlist();
            break;
        }
        case 10:
            currentCustomer = nullptr;
            cout << "Logged out successfully.\n";
            return;
        case 11:
            return; // back to main menu
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (true);
}

// -------------- MAIN --------------
int main()
{
    Shopping shop;
    shop.loadProductsOnStartup();
    shop.menu();
    shop.saveAllProducts();
    return 0;
}
