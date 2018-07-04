/*
Brainstormers 2D (Soccer Simulation League 2D)
PUBLIC SOURCE CODE RELEASE 2005
Copyright (C) 1998-2005 Neuroinformatics Group,
                        University of Osnabrueck, Germany

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include<iostream>
#include "sort.h"


Sort::Sort(int number){
  sortarray = new SortType[number];
  sortarray_maxsize = number;
  sortarray_size = 0;
}

void Sort::add(int key, float value){
  if(sortarray_size>=sortarray_maxsize){
    std::cout<<"Sort: no more entries, max: "<<sortarray_maxsize<< std::endl;
    return;
  }
  sortarray[sortarray_size].value = value;

  sortarray[sortarray_size++].key = key;
    
}

void Sort::reset(){
  for(int i= 0;i<sortarray_size;i++){
    sortarray[i].value = 0;
    sortarray[i].key = 0;
  }
  sortarray_size = 0;
}

void Sort::do_sort(int type ){
  SortType tmp;

  for(int i= 0;i<sortarray_size;i++){
    for(int j= i+1;j<sortarray_size;j++){
      if((type == 0 && sortarray[j].value < sortarray[i].value) ||
	 (type == 1 && sortarray[j].value > sortarray[i].value)){
	//switch
	tmp = sortarray[i]; 
	sortarray[i]=sortarray[j];
	sortarray[j]=tmp;
      }
    }
  }

#if 0
    for(int i= 0;i<sortarray_maxsize;i++){
    cout<<i<<" "<<sortarray[i].key<<" "<<sortarray[i].value<<endl;
  }
#endif
}

int Sort::get_key(int pos){
  if(pos<0 || pos >= sortarray_size)
    return -1;
  return sortarray[pos].key;
}

float Sort::get_value(int pos){
  if(pos<0 || pos >= sortarray_size)
    return -1;
  return sortarray[pos].value;

}

#if 0
void main(){
  Sort sort = Sort(10);

  sort.add(5,8.2);
  sort.add(2,8.2);
  sort.add(4,1.2);
  sort.add(3,1.2);
  sort.add(9,1.2);
  sort.add(8,1.2);
  sort.add(7,3.2);
  
  //sort.do_sort();
  sort.do_sort(1);
  cout<<"sorted keys and values"<<endl;
  for(int i= 0;i<10;i++){
    cout<<i<<" "<<sort.get_key(i)<<" "<<sort.get_value(i)<<endl;
  }
}
#endif
