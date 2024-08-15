# convert_objs_to_abc tool

This is a tool which is to convert obj sequences to abc file. This code is based on [Alembic Library](https://github.com/alembic/alembic). I uploaded the builded lib in lib directory.(the lib is build in windows 7 64 bit system)

## useage  
### how to compile  
clone the code to local, and open it in vs 2019, compile, done!

### how to use  
befor using, shold know following:
* Let the obj sequence name like 0000.obj 0001.obj and so on
* Let the obj sequence have the same topology

usage
download Release.zip and unzip.
```python
Objs2Abc.exe -i <objs_directory> -o <output_name.abc> -f <framerate>
```
example  
```python
Objs2Abc.exe -i ..\..\head-poses -o output.abc -f 24
```
![](./imgs/1.png)  

result:  
![](./imgs/0.gif)


