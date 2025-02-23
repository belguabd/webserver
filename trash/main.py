with open("/Users/emagueri/Desktop/download.jpg", "rb") as f:
    data = f.read()
    print(data[:500])  # Print the first 500 bytes as ASCII
