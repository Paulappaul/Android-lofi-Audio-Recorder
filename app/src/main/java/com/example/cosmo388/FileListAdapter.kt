package com.example.cosmo388

import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.example.oboe.R

data class FileName(val name: String)

class FileListAdapter(private val fileNames: MutableList<FileName>) :
    RecyclerView.Adapter<FileListAdapter.FileViewHolder>() {

    class FileViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        val fileNameTextView: TextView = itemView.findViewById(R.id.tv_systemfile)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): FileViewHolder {
        val itemView = LayoutInflater.from(parent.context)
            .inflate(R.layout.menuadapter, parent, false)
        return FileViewHolder(itemView)
    }

    override fun onBindViewHolder(holder: FileViewHolder, position: Int) {
        val fileName = fileNames[position]
        holder.fileNameTextView.text = fileName.name
    }

    override fun getItemCount(): Int {
        return fileNames.size
    }

    fun updateData(newFileNames: List<FileName>) {
        Log.i("File list Adapter: ", "Updating data: $newFileNames")
        fileNames.clear()
        fileNames.addAll(newFileNames)
        notifyDataSetChanged()
    }
}
